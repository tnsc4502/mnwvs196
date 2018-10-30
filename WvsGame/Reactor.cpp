#include "Reactor.h"
#include "Field.h"
#include "ReactorTemplate.h"
#include "ReactorPool.h"
#include "NPC.h"
#include "User.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\ReactorPacketFlags.hpp"
#include "..\WvsLib\Random\Rand32.h"

int Reactor::GetHitTypePriorityLevel(int nOption, int nType)
{
	int v2; // ecx@1
	unsigned int v3; // eax@1
	int result; // eax@3
	int v5; // ecx@7

	v2 = nType;
	v3 = nOption & 1;
	if (nOption & 2)
	{
		if (!nType)
			return 1;
	}
	else
	{
		if (!nType)
			return 2;
		if (nType == 1)
			return v3 != 0 ? -1 : 1;
		v2 = nType - 2;
		if (nType == 2)
			return v3 != 0 ? 1 : -1;
	}
	v5 = v2 - 1;
	if (v5)
	{
		if (v5 == 1)
			result = (v3 != 0) - 1;
		else
			result = -1;
	}
	else
		result = -(v3 != 0);
	return result;
}

Reactor::Reactor(ReactorTemplate* pTemplate, Field* pField)
	: m_pTemplate(pTemplate),
	  m_pField(pField)
{
}

Reactor::~Reactor()
{
}

void Reactor::MakeEnterFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2((short)ReactorSendPacketFlag::Reactor_OnReactorEnterField);
	oPacket->Encode4(m_nFieldObjectID);
	oPacket->Encode4(m_nTemplateID);
	oPacket->Encode1(m_nState);
	oPacket->Encode2((short)m_ptPos.x);
	oPacket->Encode2((short)m_ptPos.y);
	oPacket->Encode1(m_bFlip);
	oPacket->EncodeStr(m_sReactorName);
}

void Reactor::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2((short)ReactorSendPacketFlag::Reactor_OnReactorLeaveField);
	oPacket->Encode4(m_nFieldObjectID);
	oPacket->Encode1(m_nState);
	oPacket->Encode2((short)m_ptPos.x);
	oPacket->Encode2((short)m_ptPos.y);
}

int Reactor::GetHitDelay(int nEventIdx)
{
	auto pInfo = m_pTemplate->GetStateInfo(m_nState);
	auto pEventInfo = m_pTemplate->GetEventInfo(m_nState, nEventIdx);
	int nRet = 0;
	if (pInfo && pEventInfo)
	{
		int v4 = 0;
		if (nEventIdx >= 0
			&& (v4 = pEventInfo->m_tHitDelay, v4 < 0)
			&& (v4 = pInfo->m_tHitDelay, pInfo->m_tHitDelay < 0)
			&& (v4 = m_pTemplate->m_tHitDelay, v4 < 0))
			nRet = 0;
		else
			nRet = v4;
	}
	return nRet;
}

void Reactor::OnHit(User * pUser, InPacket * iPacket)
{
	int nOption = iPacket->Decode4();
	int tActionDelay = iPacket->Decode2();

	auto aInfo = m_pTemplate->m_aStateInfo;
	if ((m_nState >= aInfo.size()) || aInfo[m_nState].m_aEventInfo.size() == 0)
		return;

	auto& info = aInfo[m_nState];
	int nLevel = -1, nEventIdx = 0, nIterIdx = 0;
	for (auto& eventInfo : info.m_aEventInfo)
	{
		int nRet = GetHitTypePriorityLevel(nOption, eventInfo.m_nType);
		if (nRet != -1 && (nRet < nLevel || nLevel == -1))
		{
			nLevel = nRet;
			nEventIdx = nIterIdx;
		}
		if (nRet == 0)
			break;
		++nIterIdx;
	}
	if (nLevel != -1)
	{
		if (!m_nHitCount && !m_nOwnerID)
		{
			m_nOwnerID = pUser->GetUserID();
			/*
			If the user has a party, set ownType = 1
			*/
			m_nOwnPartyID = 0;
		}
		if (m_pTemplate->m_nReqHitCount > 0)
			++m_nHitCount;
		if (m_nHitCount >= m_pTemplate->m_nReqHitCount)
		{
			SetState(nEventIdx, tActionDelay);
			m_nHitCount = 0;
			m_nLastHitCharacterID = pUser->GetUserID();
		}
		m_tLastHit = GameDateTime::GetTime();
	}
}

void Reactor::SetState(int nEventIdx, int tActionDelay)
{
	auto tCur = GameDateTime::GetTime();
	m_tStateEnd = tCur + GetHitDelay(nEventIdx) + tActionDelay;

	auto pInfo = m_pTemplate->GetStateInfo(m_nState);
	auto pEventInfo = m_pTemplate->GetEventInfo(m_nState, nEventIdx);
	if (pInfo == nullptr || pEventInfo == nullptr)
		m_nState = m_pTemplate->m_aStateInfo[m_nState].m_aEventInfo[nEventIdx].m_nStateToBe;
	else
		m_nState = (m_nState + 1) % m_pTemplate->m_aStateInfo.size();
	pInfo = m_pTemplate->GetStateInfo(m_nState);
	m_tTimeout = pInfo->m_tTimeout;
	bool bRemove = (pInfo->m_aEventInfo.size() == 0) && (m_pField->GetFieldSet() == nullptr || m_pTemplate->m_bRemoveInFieldSet);
	if (bRemove)
		m_pField->GetReactorPool()->RemoveReactor(this);
	else
	{
		OutPacket oPacket;
		MakeStateChangePacket(&oPacket, tActionDelay, nEventIdx);
		m_pField->BroadcastPacket(&oPacket);
	}
}

void Reactor::MakeStateChangePacket(OutPacket * oPacket, int tActionDelay, int nProperEventIdx)
{
	oPacket->Encode2((short)ReactorSendPacketFlag::Reactor_OnReactorChangeState);
	oPacket->Encode4(m_nFieldObjectID);
	oPacket->Encode1(m_nState);
	oPacket->Encode2(m_ptPos.x);
	oPacket->Encode2(m_ptPos.y);
	oPacket->Encode2(tActionDelay);
	oPacket->Encode1(nProperEventIdx);
	oPacket->Encode1((m_tStateEnd - GameDateTime::GetTime() + 99) / 100);
	oPacket->Encode4(m_nOwnerID);
}

void Reactor::SetRemoved()
{
	auto pGen = ((ReactorPool::ReactorGen*) m_pReactorGen);
	if (pGen)
	{
		if (pGen->tRegenInterval && --pGen->nReactorCount)
		{
			int tCur = GameDateTime::GetTime();
			int tBase = 6 * pGen->tRegenInterval / 10;
			pGen->tRegenAfter = tCur + (7 * pGen->tRegenAfter / 10) + (tBase > 0 ? Rand32::GetInstance()->Random() % tBase : 0);
		}
	}
}
