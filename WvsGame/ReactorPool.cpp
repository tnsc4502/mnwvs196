#include "ReactorPool.h"
#include "Field.h"
#include "User.h"
#include "ReactorTemplate.h"
#include "Reactor.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Net\PacketFlags\ReactorPacketFlags.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include <mutex>

std::atomic<int> ReactorPool::ReactorGen::stGenID = 1000;

ReactorPool::ReactorPool()
{
}


ReactorPool::~ReactorPool()
{
}

void ReactorPool::Init(Field * pField, void * pImg)
{
	m_pField = pField;
	auto& reactorImg = *((WZ::Node*)pImg);
	for (auto& reactor : reactorImg)
	{
		ReactorGen newGen;
		newGen.nTemplateID = atoi(((std::string)reactor["id"]).c_str());
		newGen.nX = (int)reactor["x"];
		newGen.nY = (int)reactor["y"];
		newGen.bFlip = (int)reactor["f"] == 0 ? false : true;
		newGen.tRegenInterval = (int)reactor["reactorTime"] * 1000;
		newGen.sName = (std::string)reactor["name"];

		m_aReactorGen.push_back(std::move(newGen));
	}
	TryCreateReactor(true);
}

void ReactorPool::TryCreateReactor(bool bReset)
{
	std::lock_guard<std::mutex> lock(m_mtxReactorPoolMutex);
	std::vector<ReactorGen*> aGen;

	auto tCur = GameDateTime::GetTime();
	if (!bReset && (int)(tCur - m_tLastCreateReactorTime) < UPDATE_INTERVAL)
		return;

	for (auto& gen : m_aReactorGen)
	{
		if (!bReset && gen.tRegenInterval <= 0)
			continue;
		if (gen.nReactorCount || tCur - gen.tRegenAfter < 0)
			continue;
		aGen.push_back(&gen);
	}

	while (aGen.size() > 0)
	{
		int nIdx = 0;
		if (!(aGen[nIdx]->tRegenInterval))
			nIdx = ((int)Rand32::GetInstance()->Random()) % aGen.size();
		auto pGen = aGen[nIdx];
		aGen.erase(aGen.begin() + nIdx);
		CreateReactor(pGen);
	}
	m_tLastCreateReactorTime = tCur;
}

void ReactorPool::CreateReactor(ReactorGen * pPrg)
{
	auto pTemplate = ReactorTemplate::GetReactorTemplate(pPrg->nTemplateID);
	if (pTemplate == nullptr)
		return;

	auto pReactor = AllocObjCtor(Reactor)(pTemplate, m_pField);
	pReactor->m_nTemplateID = pPrg->nTemplateID;
	pReactor->m_ptPos.x = pPrg->nX;
	pReactor->m_ptPos.y = pPrg->nY;
	pReactor->m_bFlip = pPrg->bFlip;
	pReactor->m_nHitCount = 0;
	pReactor->m_nState = 0;
	pReactor->m_nOldState = 0;
	pReactor->m_tStateEnd = GameDateTime::GetTime();
	pReactor->m_sReactorName = pPrg->sName;
	pReactor->m_nFieldObjectID = ++ReactorGen::stGenID;
	pReactor->m_pReactorGen = pPrg;
	++pPrg->nReactorCount;

	m_mReactor[pReactor->m_nFieldObjectID] = pReactor;
	m_mReactorName[pReactor->m_sReactorName] = pReactor->m_nFieldObjectID;

	OutPacket oPacket;
	pReactor->MakeEnterFieldPacket(&oPacket);
	m_pField->BroadcastPacket(&oPacket);
}

void ReactorPool::OnEnter(User * pUser)
{
	std::lock_guard<std::mutex> lock(m_mtxReactorPoolMutex);
	for (auto& pReactor : m_mReactor)
	{
		OutPacket oPacket;
		pReactor.second->MakeEnterFieldPacket(&oPacket);
		pUser->SendPacket(&oPacket);
	}
}

void ReactorPool::OnPacket(User *pUser, int nType, InPacket * iPacket)
{
	switch (nType)
	{
		case ReactorRecvPacketFlag::Reactor_OnHitReactor:
			OnHit(pUser, iPacket);
			break;
		case ReactorRecvPacketFlag::Reactor_OnClickReactor:
			break;
		case ReactorRecvPacketFlag::Reactor_OnKey:
			break;
	}
}

void ReactorPool::OnHit(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::mutex> lock(m_mtxReactorPoolMutex);
	int nID = iPacket->Decode4();
	auto pFindIter = m_mReactor.find(nID);
	if (pFindIter == m_mReactor.end())
		return;
	pFindIter->second->OnHit(pUser, iPacket);
}

void ReactorPool::RemoveReactor(Reactor * pReactor)
{
	pReactor->SetRemoved();
	OutPacket oPacket;
	pReactor->MakeLeaveFieldPacket(&oPacket);
	m_pField->BroadcastPacket(&oPacket);
	m_mReactorName.erase(((ReactorGen*)pReactor->m_pReactorGen)->sName);
	m_mReactor.erase(pReactor->m_nFieldObjectID);
	FreeObj( pReactor );
}

void ReactorPool::Update(int tCur)
{
	TryCreateReactor(false);
}
