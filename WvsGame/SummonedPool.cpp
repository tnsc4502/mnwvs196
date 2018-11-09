#include "SummonedPool.h"
#include "Summoned.h"
#include "Field.h"
#include "User.h"
#include "WvsPhysicalSpace2D.h"
#include "StaticFoothold.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

SummonedPool::SummonedPool(Field *pField)
{
	m_pField = pField;
	m_nSummonedIdCounter = 1000;
}

SummonedPool::~SummonedPool()
{
}

std::mutex & SummonedPool::GetSummonedPoolLock()
{
	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	return m_mtxSummonedLock;
}

Summoned * SummonedPool::GetSummoned(int nFieldObjID)
{
	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	for (auto& pSummoned : m_lSummoned)
		if (pSummoned->GetFieldObjectID() == nFieldObjID)
			return pSummoned;
	return nullptr;
}

bool SummonedPool::CreateSummoned(User* pUser, Summoned * pSummoned, const FieldPoint & pt)
{
	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	pSummoned->SetFieldObjectID(m_nSummonedIdCounter++);
	pSummoned->m_ptPos = pt;
	pSummoned->Init(pUser, pSummoned->m_nSkillID, pSummoned->m_nSLV);
	OutPacket oPacket;
	pSummoned->MakeEnterFieldPacket(&oPacket);
	m_pField->BroadcastPacket(&oPacket);
	m_lSummoned.push_back(pSummoned);

	return true;
}

Summoned * SummonedPool::CreateSummoned(User* pUser, int nSkillID, int nSLV, const FieldPoint & pt)
{
	int nX = pUser->GetPosX();
	int nY = pUser->GetPosY() - 5;
	auto pFH = m_pField->GetSpace2D()->GetFootholdUnderneath(nX, nY, &(nY));
	if (pFH)
	{
		Summoned *pRet = AllocObj(Summoned);
		pRet->SetFh(pFH->GetSN());
		pRet->m_ptPos.x = nX;
		pRet->m_ptPos.y = nY;
		pRet->Init(pUser, nSkillID, nSLV);
		pRet->SetFieldObjectID(m_nSummonedIdCounter++);
		OutPacket oPacket;
		pRet->MakeEnterFieldPacket(&oPacket);
		m_pField->BroadcastPacket(&oPacket);

		m_lSummoned.push_back(pRet);
		return pRet;
	}

	return nullptr;
}

void SummonedPool::RemoveSummoned(int nCharacterID, int nSkillID, int nLeaveType)
{
	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	for (int i = 0; i < m_lSummoned.size(); ++i) 
	{
		auto pSummoned = m_lSummoned[i];
		if (pSummoned->GetSkillID() == nSkillID && pSummoned->GetOwnerID() == nCharacterID)
		{
			m_lSummoned.erase(m_lSummoned.begin() + i);
			OutPacket oPacket;
			pSummoned->MakeLeaveFieldPacket(&oPacket);
			m_pField->SplitSendPacket(&oPacket, pSummoned->m_pOwner);
			if(nLeaveType != Summoned::eLeave_TransferField)
				FreeObj(pSummoned);
			return;
		}
	}
}
