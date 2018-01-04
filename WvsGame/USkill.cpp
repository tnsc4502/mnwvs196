#include "USkill.h"
#include "..\Common\Net\InPacket.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_SkillRecord.h"

#include "User.h"
#include "TemporaryStat.h"
#include "SecondaryStat.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "SkillInfo.h"
#include "QWUSkillRecord.h"

#include "WvsGameConstants.hpp"
#include "Utility\DateTime\GameDateTime.h"

void USkill::OnSkillUseRequest(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::mutex> userGuard(pUser->GetLock());
	int tRequestTime = iPacket->Decode4();
	int nSkillID = iPacket->Decode4();
	int nSpiritJavelinItemID = 0;
	int nSLV = iPacket->Decode1();
	auto pSkillEntry = SkillInfo::GetInstance()->GetSkillByID(nSkillID);
	auto pSkillRecord = pUser->GetCharacterData()->GetSkill(nSkillID);
	if (pSkillEntry == nullptr || pSkillRecord == nullptr)
	{
		SendFailPacket(pUser);
		return;
	}
	nSLV = nSLV > pSkillRecord->nSLV ? pSkillRecord->nSLV : nSLV;
	if (!pUser->GetField() || nSLV <= 0) 
	{
		SendFailPacket(pUser);
		return;
	}
	DoActiveSkill_SelfStatChange(pUser, pSkillEntry, nSLV, iPacket, 0);
}

void USkill::OnSkillUpRequest(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::mutex> userGuard(pUser->GetLock());
	int tTime = iPacket->Decode4();
	int nSkillID = iPacket->Decode4();
	int nAmount = 1;
	if (iPacket->GetPacketSize() > 8)
		nAmount = iPacket->Decode1();
	if (nAmount <= 0)
		nAmount = 1;
	std::vector<GW_SkillRecord*> aChange;
	if (QWUSkillRecord::SkillUp(
		pUser,
		nSkillID,
		nAmount,
		true,
		aChange))
	{
		pUser->ValidateStat();
		pUser->SendCharacterStat(false, BasicStat::BS_SP);
	}
	QWUSkillRecord::SendCharacterSkillRecord(pUser, aChange);
}

void USkill::OnSkillPrepareRequest(User * pUser, InPacket * iPacket)
{
}

void USkill::OnSkillCancelRequest(User * pUser, InPacket * iPacket)
{
}

void USkill::SendFailPacket(User* pUser)
{
}

void USkill::DoActiveSkill_SelfStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, int nOptionValue)
{
	auto pSkillLVLData = pSkill->GetLevelData(nSLV);
	int nSkillID = pSkill->GetSkillID();
	int nDuration = pSkillLVLData->m_nTime * 1000;
	int tDelay = iPacket->Decode2();
	auto pSS = pUser->GetSecondaryStat();
	if (!pSkillLVLData) 
	{
		printf("[USkill::DoActiveSkill_SelfStatChange]異常的技能資訊，技能ID = %d，技能等級 = %d\n", pSkill->GetSkillID(), nSLV);
		return;
	}
	auto tsFlag = TemporaryStat::TS_Flag::GetDefault();

	if (pSkillLVLData->m_nPad > 0)
	{
		tsFlag |= GET_TS_FLAG(PAD);
		pSS->nPAD = pSkillLVLData->m_nPad;
		pSS->rPAD = nSkillID;
		pSS->tPAD = nDuration;
	}
	if (pSkillLVLData->m_nPdd > 0)
	{
		tsFlag |= GET_TS_FLAG(PDD);
		pSS->nPDD = pSkillLVLData->m_nPdd;
		pSS->rPDD = nSkillID;
		pSS->tPDD = nDuration;
	}
	if (pSkillLVLData->m_nMdd > 0)
	{
		tsFlag |= GET_TS_FLAG(MDD);
		pSS->nMDD = pSkillLVLData->m_nMdd;
		pSS->rMDD = nSkillID;
		pSS->tMDD = nDuration;
	}
	if (pSkillLVLData->m_nMad > 0)
	{
		tsFlag |= GET_TS_FLAG(MAD);
		pSS->nMAD = pSkillLVLData->m_nMad;
		pSS->rMAD = nSkillID;
		pSS->tMAD = nDuration;
	}
	if (pSkillLVLData->m_nAcc > 0)
	{
		tsFlag |= GET_TS_FLAG(ACC);
		pSS->nACC = pSkillLVLData->m_nAcc;
		pSS->rACC = nSkillID;
		pSS->tACC = nDuration;
	}
	if (pSkillLVLData->m_nEva > 0)
	{
		tsFlag |= GET_TS_FLAG(EVA);
		pSS->nEVA = pSkillLVLData->m_nEva;
		pSS->rEVA = nSkillID;
		pSS->tEVA = nDuration;
	}
	if (pSkillLVLData->m_nCraft > 0)
	{
		tsFlag |= GET_TS_FLAG(Craft);
		pSS->nCraft = pSkillLVLData->m_nCraft;
		pSS->rCraft = nSkillID;
		pSS->tCraft = nDuration;
	}
	if (pSkillLVLData->m_nSpeed > 0)
	{
		tsFlag |= GET_TS_FLAG(Speed);
		pSS->nSpeed = pSkillLVLData->m_nSpeed;
		pSS->rSpeed = nSkillID;
		pSS->tSpeed = nDuration;
	}
	if (pSkillLVLData->m_nJump > 0)
	{
		tsFlag |= GET_TS_FLAG(Jump);
		pSS->nJump = pSkillLVLData->m_nJump;
		pSS->rJump = nSkillID;
		pSS->tJump = nDuration;
	}
	switch (nSkillID)
	{
		case 2111008:
		{
			tsFlag |= GET_TS_FLAG(ElementalReset);
			pSS->nElementalReset = pSkillLVLData->m_nX;
			pSS->rElementalReset = nSkillID;
			pSS->tElementalReset = nDuration;
		}
		case 2121052:
		{
			tsFlag |= GET_TS_FLAG(FireAura);
			pSS->nFireAura = pSkillLVLData->m_nX;
			pSS->rFireAura = nSkillID;
			pSS->tFireAura = nDuration;
		}
	}
	pUser->ValidateStat();
	long long int liFlag = 0;
	liFlag |= BasicStat::BasicStatFlag::BS_MaxHP;
	liFlag |= BasicStat::BasicStatFlag::BS_MaxMP;
	pUser->SendTemporaryStatSet(tsFlag, tDelay);
	pUser->SendCharacterStat(true, liFlag);
}

void USkill::DoActiveSkill_WeaponBooster(User* pUser, const SkillEntry * pSkill, int nSLV, int nWT1, int nWT2)
{
}

void USkill::DoActiveSkill_TownPortal(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
}

void USkill::DoActiveSkill_PartyStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
}

void USkill::DoActiveSkill_MobStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, int bSendResult)
{
}

void USkill::DoActiveSkill_Summon(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
}

void USkill::DoActiveSkill_SmokeShell(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
}
