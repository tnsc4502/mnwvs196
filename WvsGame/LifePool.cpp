#include "LifePool.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\MobPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"

#include "..\WvsLib\Logger\WvsLogger.h"

#include "User.h"
#include "MobTemplate.h"
#include "Field.h"
#include "Controller.h"
#include "SkillEntry.h"
#include "Drop.h"
#include "AttackInfo.h"
#include "SecondaryStat.h"
#include "NpcTemplate.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"

#include <cmath>


LifePool::LifePool()
	: m_pCtrlNull(AllocObjCtor(Controller)(nullptr))
{
}


LifePool::~LifePool()
{
	for (auto& p : m_aMobGen)
		FreeObj( p.second );
	for (auto& p : m_aNpcGen)
		FreeObj( p.second );
	for (auto& p : m_hCtrl)
		FreeObj( p.second );
	FreeObj( m_pCtrlNull );
}

void LifePool::Init(Field* pField, int nFieldID)
{
	m_pField = pField;

	int nSizeX = 1920;
	int nSizeY = 1080; //I dont know
	int nGenSize = (int)(((double)nSizeX * nSizeY) * 0.0000048125f);
	if (nGenSize < 1)
		nGenSize = 1;
	else if (nGenSize >= MAX_MOB_GEN)
		nGenSize = MAX_MOB_GEN;
	m_nMobCapacityMin = nGenSize;
	m_nMobCapacityMax = nGenSize * 2 * pField->GetMobRate();

	auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"]
		["Map" + std::to_string(nFieldID / 100000000)]
		[StringUtility::LeftPadding(std::to_string(nFieldID), 9, '0')];

	auto& lifeData = mapWz["life"];
	for (auto& node : lifeData)
	{
		const auto &typeFlag = (std::string)node["type"];
		if (typeFlag == "n")
			LoadNpcData(node);
		else if (typeFlag == "m")
			LoadMobData(node);
	}	

	//強制生成所有NPC
	for (auto& npc : m_lNpc)
		CreateNpc(npc);

	TryCreateMob(false);
	//mapWz.ReleaseData();
}

void LifePool::SetFieldObjAttribute(FieldObj* pFieldObj, WZ::Node& dataNode)
{
	try {
		pFieldObj->SetPosX(dataNode["x"]);
		pFieldObj->SetPosY(dataNode["y"]);
		pFieldObj->SetF(dataNode["f"]);
		pFieldObj->SetHide(dataNode["hide"]);
		pFieldObj->SetFh(dataNode["fh"]);
		pFieldObj->SetCy(dataNode["cy"]);
		pFieldObj->SetRx0(dataNode["rx0"]);
		pFieldObj->SetRx1(dataNode["rx1"]);
		pFieldObj->SetTemplateID(atoi(((std::string)dataNode["id"]).c_str()));
	}
	catch (std::exception& e) {
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "讀取地圖物件發生錯誤，訊息:%s\n", e.what());
	}
}

void LifePool::SetMaxMobCapacity(int max)
{
	m_nMobCapacityMax = max;
}

int LifePool::GetMaxMobCapacity() const
{
	return m_nMobCapacityMax;
}

void LifePool::LoadNpcData(WZ::Node& dataNode)
{
	Npc npc;
	SetFieldObjAttribute(&npc, dataNode);
	npc.SetTemplate(NpcTemplate::GetInstance()->GetNpcTemplate(npc.GetTemplateID()));
	m_lNpc.push_back(npc);
}

void LifePool::LoadMobData(WZ::Node& dataNode)
{
	Mob mob;
	SetFieldObjAttribute(&mob, dataNode);
	mob.SetMobTemplate(MobTemplate::GetMobTemplate(mob.GetTemplateID()));
	//MobTemplate::GetMobTemplate(mob.GetTemplateID());
	m_lMob.push_back(mob);
}

void LifePool::CreateNpc(const Npc& npc)
{
	std::lock_guard<std::mutex> lock(m_lifePoolMutex);
	Npc* newNpc = AllocObj(Npc);
	*newNpc = npc; //Should notice pointer data assignment
	newNpc->SetFieldObjectID(atomicObjectCounter++);
	m_aNpcGen.insert({ newNpc->GetFieldObjectID(), newNpc });
}

void LifePool::TryCreateMob(bool reset)
{
	std::lock_guard<std::mutex> lock(m_lifePoolMutex);
	/*
	if reset, kill all monsters and respawns
	*/
	if(m_lMob.size() > 0)
		for (int i = 0; i < m_nMobCapacityMax - (m_aMobGen.size()); ++i) 
		{
			auto& mob = m_lMob[rand() % m_lMob.size()];
			CreateMob(mob, mob.GetPosX(), mob.GetPosY(), mob.GetFh(), 0, -2, 0, 0, 0, nullptr);
		}
}

void LifePool::CreateMob(const Mob& mob, int x, int y, int fh, int bNoDropPriority, int nType, unsigned int dwOption, int bLeft, int nMobType, Controller* pOwner)
{
	Controller* pController = pOwner;
	if (m_hCtrl.size() > 0)
		pController = m_hCtrl.begin()->second;

	if (pController != nullptr 
		&& (pController->GetMobCtrlCount() + pController->GetNpcCtrlCount() - (pController->GetMobCtrlCount() != 0) >= 50)
		&& (nType != -3 || nMobType != 2 || !GiveUpMobController(pController)))
		pController = nullptr;

	if (pController && pController != this->m_pCtrlNull)
	{
		Mob* newMob = AllocObj( Mob );
		*newMob = mob;
		newMob->SetField(m_pField);
		newMob->SetFieldObjectID(atomicObjectCounter++);

		int moveAbility = newMob->GetMobTemplate()->m_nMoveAbility;

		newMob->SetHP(newMob->GetMobTemplate()->m_lnMaxHP);
		newMob->SetMP((int)newMob->GetMobTemplate()->m_lnMaxMP);
		newMob->SetMovePosition(x, y, bLeft & 1 | 2 * (moveAbility == 3 ? 6 : (moveAbility == 0 ? 1 : 0) + 1), fh);
		newMob->SetMoveAction(5); //怪物 = 5 ?

		OutPacket createMobPacket;
		newMob->MakeEnterFieldPacket(&createMobPacket);
		m_pField->BroadcastPacket(&createMobPacket);

		newMob->SetController(pController);
		newMob->SendChangeControllerPacket(pController->GetUser(), 1);
		pController->AddCtrlMob(newMob);

		//19/05/07 +
		UpdateCtrlHeap(pController);
		m_aMobGen.insert({ newMob->GetFieldObjectID(), newMob });
	}
}

void LifePool::RemoveMob(Mob * pMob)
{
	if (pMob == nullptr)
		return;
	auto pController = pMob->GetController();
	if (pController->GetUser() != nullptr)
	{
		pController->RemoveCtrlMob(pMob);
		pMob->SendReleaseControllPacket(pController->GetUser(), pMob->GetFieldObjectID());
	}
	else
		m_pCtrlNull->RemoveCtrlMob(pMob);
	OutPacket oPacket;
	pMob->MakeLeaveFieldPacket(&oPacket);
	m_pField->SplitSendPacket(&oPacket, nullptr);
	m_aMobGen.erase(pMob->GetFieldObjectID());
	FreeObj( pMob );
}

void LifePool::OnEnter(User *pUser)
{
	std::lock_guard<std::mutex> lock(m_lifePoolMutex);
	InsertController(pUser);

	for (auto& npc : m_aNpcGen)
	{
		OutPacket oPacket;
		npc.second->MakeEnterFieldPacket(&oPacket);
		npc.second->SendChangeControllerPacket(pUser);
		pUser->SendPacket(&oPacket);
	}
	//WvsLogger::LogFormat("LifePool::OnEnter : Total Mob = %d\n", m_aMobGen.size());
	for (auto& mob : m_aMobGen)
	{
		OutPacket oPacket;
		mob.second->MakeEnterFieldPacket(&oPacket);
		pUser->SendPacket(&oPacket);
	}
	//WvsLogger::LogFormat("LifePool::OnEnter : Total Controlled = %d Null Controlled = %d\n size of m_hCtrl = %d", m_mController[pUser->GetUserID()]->second->GetTotalControlledCount(), m_pCtrlNull->GetTotalControlledCount(), m_hCtrl.size());
}

void LifePool::InsertController(User* pUser)
{
	Controller* controller = AllocObjCtor(Controller)(pUser) ;
	auto& iter = m_hCtrl.insert({ 0, controller });
	m_mController.insert({ pUser->GetUserID(), iter });
	RedistributeLife();
}

void LifePool::RemoveController(User* pUser)
{
	std::lock_guard<std::mutex> lock(m_lifePoolMutex);
	if (m_mController.size() == 0)
		return;

	//找到pUser對應的iterator
	auto& iter = m_mController.find(pUser->GetUserID());

	//根據iterator找到controller指標
	auto pController = iter->second->second;

	//從hCtrl中移除此controller
	m_hCtrl.erase(iter->second);

	//從pUser中移除iter
	m_mController.erase(iter);

	auto& controlled = pController->GetMobCtrlList();
	for (auto pMob : controlled)
	{
		Controller* pCtrlNew = m_pCtrlNull;
		if (m_hCtrl.size() > 0)
			pCtrlNew = m_hCtrl.begin()->second;
		pMob->SendChangeControllerPacket(pController->GetUser(), 0);
		pMob->SetController(pCtrlNew);
		pCtrlNew->AddCtrlMob(pMob);
		if (pCtrlNew != m_pCtrlNull)
		{
			pMob->SendChangeControllerPacket(pCtrlNew->GetUser(), 1);
			UpdateCtrlHeap(pCtrlNew);
		}
	}

	//銷毀
	FreeObj( pController );
}

void LifePool::UpdateCtrlHeap(Controller * pController)
{
	//根據controller找到對應的pUser
	auto pUser = pController->GetUser();

	//找到pUser對應的iterator
	auto& iter = m_mController.find(pUser->GetUserID());


	//從hCtrl中移除此controller，並重新插入 [新的數量為key]
	m_hCtrl.erase(iter->second);
	m_mController[pUser->GetUserID()] = m_hCtrl.insert({ pController->GetTotalControlledCount(), pController });
}

bool LifePool::GiveUpMobController(Controller * pController)
{
	return false;
}

void LifePool::RedistributeLife()
{
	Controller* pCtrl = nullptr;
	int nCtrlCount = (int)m_hCtrl.size();
	if (nCtrlCount > 0)
	{
		auto& nonControlled = m_pCtrlNull->GetMobCtrlList();
		//for (auto pMob : nonControlled)
		for(auto iter = nonControlled.begin(); iter != nonControlled.end(); )
		{
			auto pMob = *iter;
			pCtrl = m_hCtrl.begin()->second;

			//控制NPC與怪物數量總和超過50，重新配置
			if (pCtrl->GetTotalControlledCount() >= 50)
				break;
			pCtrl->AddCtrlMob(pMob);
			pMob->SetController(pCtrl);
			pMob->SendChangeControllerPacket(pCtrl->GetUser(), 1);
			UpdateCtrlHeap(pCtrl);

			m_pCtrlNull->RemoveCtrlMob(pMob);
			iter = nonControlled.begin();
		}
		//NPC

		Controller* minCtrl, *maxCtrl;
		int nMaxNpcCtrl, nMaxMobCtrl, nMinNpcCtrl, nMinMobCtrl;
		//重新調配每個人的怪物控制權
		if (nCtrlCount >= 2) //至少一個minCtrl與maxCtrl
		{
			while (true) 
			{
				minCtrl = m_hCtrl.begin()->second;
				maxCtrl = m_hCtrl.rbegin()->second;
				nMaxNpcCtrl = maxCtrl->GetNpcCtrlCount();
				nMaxMobCtrl = maxCtrl->GetMobCtrlCount();
				nMinNpcCtrl = minCtrl->GetNpcCtrlCount();
				nMinMobCtrl = minCtrl->GetMobCtrlCount();
				WvsLogger::LogFormat("Min Ctrl User = %d(%d), Max Ctrl User = %d(%d)\n", minCtrl->GetUser()->GetUserID(), nMinMobCtrl, maxCtrl->GetUser()->GetUserID(), nMaxMobCtrl);
				//已經足夠平衡不需要再重新配給
				if ((nMaxNpcCtrl + nMaxMobCtrl - (nMaxMobCtrl != 0) <= (nMinNpcCtrl - (nMinMobCtrl != 0) + nMinMobCtrl + 1))
					|| ((nMaxNpcCtrl + nMaxMobCtrl - (nMaxMobCtrl != 0)) <= 10))
					break;
				WvsLogger::LogFormat("Unbalanced.\n", minCtrl->GetUser()->GetUserID(), nMinMobCtrl, maxCtrl->GetUser()->GetUserID(), nMaxMobCtrl);
				Mob* pMob = *(maxCtrl->GetMobCtrlList().rbegin());
				maxCtrl->GetMobCtrlList().pop_back();
				pMob->SendChangeControllerPacket(maxCtrl->GetUser(), 0);

				minCtrl->AddCtrlMob(pMob);
				pMob->SetController(minCtrl);
				pMob->SendChangeControllerPacket(minCtrl->GetUser(), 1);
				UpdateCtrlHeap(minCtrl);
				UpdateCtrlHeap(maxCtrl);
			}
		}
	}
}

void LifePool::Update()
{
	TryCreateMob(false);
}

void LifePool::OnPacket(User * pUser, int nType, InPacket * iPacket)
{
	if (nType >= FlagMin(MobRecvPacketFlag) && nType <= FlagMax(MobRecvPacketFlag))
	{
		OnMobPacket(pUser, nType, iPacket);
	}
	else if (nType == 0x384)
		OnNpcPacket(pUser, nType, iPacket);
}

void LifePool::OnUserAttack(User * pUser, const SkillEntry * pSkill, AttackInfo * pInfo)
{
	std::lock_guard<std::mutex> mobLock(this->m_lifePoolMutex);

	OutPacket attackPacket;
	EncodeAttackInfo(pUser, pInfo, &attackPacket);
	m_pField->SplitSendPacket(&attackPacket, nullptr);

	for (const auto& dmgInfo : pInfo->m_mDmgInfo)
	{
		auto mobIter = m_aMobGen.find(dmgInfo.first);
		if (mobIter == m_aMobGen.end())
			continue;
		auto pMob = mobIter->second;
		auto& refDmgValues = dmgInfo.second;
		for (const auto& dmgValue : refDmgValues)
		{
			//printf("Monster %d Damaged : %d Total : %d\n", dmgInfo.first, dmgValue, pMob->GetMobTemplate()->m_lnMaxHP);
			pMob->OnMobHit(pUser, dmgValue, pInfo->m_nType);
			if (pMob->GetHP() <= 0)
			{
				pMob->OnMobDead(
					pMob->GetPosX(),
					pMob->GetPosY(),
					pUser->GetSecondaryStat()->nMesoUp,
					pUser->GetSecondaryStat()->nMesoUpByItem
				);
				RemoveMob(pMob);
				break;
			}
		}
	}
}

void LifePool::EncodeAttackInfo(User * pUser, AttackInfo * pInfo, OutPacket * oPacket)
{
	oPacket->Encode2(pInfo->m_nType - UserRecvPacketFlag::User_OnUserAttack_MeleeAttack + UserSendPacketFlag::UserRemote_OnMeleeAttack);
	oPacket->Encode4(pUser->GetUserID());
	oPacket->Encode1(0);
	oPacket->Encode1(pInfo->m_bAttackInfoFlag);
	oPacket->Encode1((char)200);
	if (pInfo->m_nSkillID > 0 || pInfo->m_nType == UserRecvPacketFlag::User_OnUserAttack_MagicAttack)
	{
		oPacket->Encode1(pInfo->m_nSLV);
		if (pInfo->m_nSLV > 0)
			oPacket->Encode4(pInfo->m_nSkillID);
	}
	else if (pInfo->m_nType != UserRecvPacketFlag::User_OnUserAttack_ShootAttack)
		oPacket->Encode1(0);

	if (WvsGameConstants::IsZeroSkill(pInfo->m_nSkillID) && pInfo->m_nSkillID != 100001283)
		oPacket->Encode1(0);

	if (pInfo->m_nType == UserRecvPacketFlag::User_OnUserAttack_ShootAttack)
		oPacket->Encode1(0);

	if (pInfo->m_nSkillID == 80001850)
	{
		oPacket->Encode1(0);
		oPacket->Encode4(0);
	}

	if (pInfo->m_nSkillID == 40021185 || pInfo->m_nSkillID == 42001006)
		oPacket->Encode1(0);

	if (pInfo->m_nType == UserRecvPacketFlag::User_OnUserAttack_ShootAttack ||
		pInfo->m_nType == UserRecvPacketFlag::User_OnUserAttack_MeleeAttack)
		oPacket->Encode1(0);

	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	oPacket->Encode2(pInfo->m_nDisplay);
	oPacket->Encode1((char)0xFF);
	oPacket->Encode2(0);
	oPacket->Encode2(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(pInfo->m_nAttackSpeed);
	oPacket->Encode1(0);
	oPacket->Encode4(pInfo->m_tKeyDown);
	for (const auto& dmgInfo : pInfo->m_mDmgInfo)
	{
		oPacket->Encode4(dmgInfo.first);
		oPacket->Encode1(7);
		oPacket->Encode1(0);
		oPacket->Encode1(0);
		oPacket->Encode2(0);
		if (pInfo->m_nSkillID == 80011050)
			oPacket->Encode1(0);
		for (const auto& dmgValue : dmgInfo.second)
			oPacket->Encode8(dmgValue);

		if (pInfo->m_nSkillID == 142100010
			|| pInfo->m_nSkillID == 142110003
			|| pInfo->m_nSkillID == 142110015
			|| pInfo->m_nSkillID == 142111002
			|| pInfo->m_nSkillID > 142119999 && (pInfo->m_nSkillID <= 142120002 || pInfo->m_nSkillID == 142120014)) 
			oPacket->Encode4(0);
	}
	if (pInfo->m_nSkillID == 2321001 || pInfo->m_nSkillID == 2221052 || pInfo->m_nSkillID == 11121052) 
		oPacket->Encode4(0);
	else if (pInfo->m_nSkillID == 65121052 || 
		pInfo->m_nSkillID == 101000202 || 
		pInfo->m_nSkillID == 101000102) 
	{
		oPacket->Encode4(0);
		oPacket->Encode4(0);
	}
	if (pInfo->m_nSkillID == 42100007) 
	{
		oPacket->Encode2(0);
		oPacket->Encode1(0);
	}
	if (pInfo->m_nType == UserRecvPacketFlag::User_OnUserAttack_ShootAttack)
	{
		oPacket->Encode2(pUser->GetPosX());
		oPacket->Encode2(pUser->GetPosY());
	}
	else if ((pInfo->m_nType == UserRecvPacketFlag::User_OnUserAttack_MagicAttack) && pInfo->m_tKeyDown > 0)
		oPacket->Encode4(pInfo->m_tKeyDown);
	
	if (pInfo->m_nSkillID == 5321000 || 
		pInfo->m_nSkillID == 5311001 || 
		pInfo->m_nSkillID == 5321001 ||
		pInfo->m_nSkillID == 5011002 || 
		pInfo->m_nSkillID == 5311002 || 
		pInfo->m_nSkillID == 5221013 || 
		pInfo->m_nSkillID == 5221017 || 
		pInfo->m_nSkillID == 3120019 || 
		pInfo->m_nSkillID == 3121015 || 
		pInfo->m_nSkillID == 4121017) {
		oPacket->Encode2(pUser->GetPosX());
		oPacket->Encode2(pUser->GetPosY());
	}
	if (pInfo->m_nSkillID == 40020009 || 
		pInfo->m_nSkillID == 40020010 || 
		pInfo->m_nSkillID == 40020011) {
		oPacket->Encode2(pUser->GetPosX());
		oPacket->Encode2(pUser->GetPosY());
	}
	static int aZeroBuff[20] = { 0 };
	oPacket->EncodeBuffer((unsigned char*)aZeroBuff, 80);
}

std::mutex & LifePool::GetLock()
{
	return m_lifePoolMutex;
}

Npc * LifePool::GetNpc(int nFieldObjID)
{
	auto findIter = m_aNpcGen.find(nFieldObjID);
	if (findIter != m_aNpcGen.end())
		return findIter->second;
	return nullptr;
}

Mob * LifePool::GetMob(int nFieldObjID)
{
	auto findIter = m_aMobGen.find(nFieldObjID);
	if (findIter != m_aMobGen.end())
		return findIter->second;
	return nullptr;
}

void LifePool::OnMobPacket(User * pUser, int nType, InPacket * iPacket)
{
	int dwMobID = iPacket->Decode4();
	std::lock_guard<std::mutex> lock(m_lifePoolMutex);

	auto mobIter = m_aMobGen.find(dwMobID);
	if (mobIter != m_aMobGen.end()) {
		switch (nType)
		{
		case MobRecvPacketFlag::Mob_OnMove:
			m_pField->OnMobMove(pUser, mobIter->second, iPacket);
			break;
		}
	}
	else {
		//Release Controller
	}
}

void LifePool::OnNpcPacket(User * pUser, int nType, InPacket * iPacket)
{
	std::lock_guard<std::mutex> lock(m_lifePoolMutex);
	if (nType == 0x384)
	{
		auto iterNpc = this->m_aNpcGen.find(iPacket->Decode4());
		if (iterNpc != m_aNpcGen.end())
		{
			iterNpc->second->OnUpdateLimitedInfo(pUser, iPacket);
		}
	}
}

int LifePool::GetForceAtomObjectID()
{
	return ++atomicForceAtomCounter;
}