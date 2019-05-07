#include "Mob.h"
#include "MobTemplate.h"
#include "..\Database\GW_MobReward.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "Reward.h"
#include "DropPool.h"
#include "User.h"
#include "ItemInfo.h"
#include "QWUser.h"
#include "Field.h"

#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\MobPacketFlags.hpp"

Mob::Mob()
{
}

Mob::~Mob()
{
}

void Mob::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2(MobSendPacketFlag::Mob_OnMakeEnterFieldPacket); //MobPool::SpawnMonster
	oPacket->Encode1(0);
	EncodeInitData(oPacket);
}

void Mob::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2(MobSendPacketFlag::Mob_OnMakeLeaveFieldPacket); //MobPool::SpawnMonster
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode1(1);
}

void Mob::EncodeInitData(OutPacket *oPacket, bool bIsControl)
{
	//printf("Encode Init Data oid = %d template ID = %d Is Control?%d\n", GetFieldObjectID(), GetTemplateID(), (int)bIsControl);
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode1(1); //Control Mode
	oPacket->Encode4(GetTemplateID());

	//MobStat::EncodeForcedStat
	oPacket->Encode1(m_pMobTemplate->m_bIsChangeableMob);
	if (m_pMobTemplate->m_bIsChangeableMob)
	{
		oPacket->Encode4(m_pMobTemplate->m_lnMaxHP > INT_MAX ? INT_MAX : (int)m_pMobTemplate->m_lnMaxHP);
		oPacket->Encode4(m_pMobTemplate->m_lnMaxMP > INT_MAX ? INT_MAX : (int)m_pMobTemplate->m_lnMaxMP);
		oPacket->Encode4(m_pMobTemplate->m_nEXP);
		oPacket->Encode4(m_pMobTemplate->m_nPAD); //Physical Attack
		oPacket->Encode4(m_pMobTemplate->m_nMAD); //Magical  Attack
		oPacket->Encode4(m_pMobTemplate->m_nPDRate);
		oPacket->Encode4(m_pMobTemplate->m_nMDRate);
		oPacket->Encode4(m_pMobTemplate->m_nACC);
		oPacket->Encode4(m_pMobTemplate->m_nEVA);
		oPacket->Encode4(m_pMobTemplate->m_nPushed);
		oPacket->Encode4(m_pMobTemplate->m_nSpeed);
		oPacket->Encode4(m_pMobTemplate->m_nLevel);
		oPacket->Encode4(0); // Unknown
	}
	oPacket->EncodeBuffer(nullptr, 12);

	//MobStat::EncodeTemporary
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetPosY());
	oPacket->Encode1(GetMoveAction()); // m_bMoveAction

	/*if (m_pMobTemplate->m_bIsSmartPhase)
		oPacket->Encode1(0);*/

	oPacket->Encode2(GetFh()); // Current FH m_nFootholdSN
	oPacket->Encode2(GetFh()); // Origin FH m_nHomeFoothold
	
	int spawnType = m_pMobTemplate->m_nSummonType <= 1 || m_pMobTemplate->m_nSummonType == 24 ? -2 : m_pMobTemplate->m_nSummonType;
	//printf("Spawn Type = %d\n", spawnType);
	if (!bIsControl) {
		oPacket->Encode2(spawnType /*m_pMobTemplate->m_nSummonType*/);
		if (spawnType == -3 || spawnType >= 0/*m_pMobTemplate->m_nSummonType == -3 || m_pMobTemplate->m_nSummonType >= 0*/)
			oPacket->Encode4(0); //dwSummonOption  -->  Linked ObjectID
	}
	else
		oPacket->Encode2(-1);

	oPacket->Encode1(-1); //Carnival Team
	oPacket->Encode8((int)m_pMobTemplate->m_lnMaxHP);

	oPacket->Encode4(0); //nEffectItemID
	oPacket->Encode4(0); //m_nPatrolScopeX1
	oPacket->Encode4(0); //m_nPatrolScopeX2
	oPacket->Encode4(0); //nDetectX
	oPacket->Encode4(0); //nSenseX

	oPacket->Encode4(-1); //m_nPhase
	oPacket->Encode4(-1); //m_nCurZoneDataType

	oPacket->Encode1(0); //m_dwRefImgMobID

	//oPacket->Encode1(0);
	//m_dwLifeReleaseOwnerAID = CInPacket::Decode4
	//m_sLifeReleaseOwnerName = CInPacket::DecodeStr
	//m_sLifeReleaseMobName = CInPacket::DecodeStr

	oPacket->Encode4(0); //nAfterAttack
	oPacket->Encode4(0x64); //nCurrentAction
	oPacket->Encode1(-1); //bIsLeft
	oPacket->Encode4(0); //nCount
	/*
	for(int i = 0; i < nCount; ++i)
	{
		oPacket->Encode4(0);
		oPacket->Encode4(0);
	}
	*/

	oPacket->Encode4(0); //m_nScale
	oPacket->Encode4(0); //m_nEliteGrade
	/*
	if((m_nEliteGrade & 0x80000000) == 0)
	{
		oPacket->Encode4(pvcMobActiveObj);
		for(int i = 0; i < pvcMobActiveObj; ++i)
		{
			oPacket->Encode4(first);
			oPacket->Encode4(second);
		}
		oPacket->Encode4(m_nEliteType);
	}
	*/

	oPacket->Encode1(0); //bIsShootingMobStat
	/*
	ShootingMobStat::Encode 
	  CInPacket::Eecode4(nMovePattern);
	  CInPacket::Eecode4(nMoveRange);
	  CInPacket::Eecode4(nBulletUpgrade);
	  CInPacket::Eecode4(nMoveSpeed);
	  CInPacket::Eecode4(nMoveAngle);
	*/

	oPacket->Encode1(0); //unk
	/*
		oPacket->Encode4(0);
		oPacket->Encode4(0);
	*/

	oPacket->Encode4(0); //pvcMobActiveObj
	/*
	for(int i = 0; i < pvcMobActiveObj; ++i)
	{
		oPacket->Encode4(nType);
		oPacket->Encode4(m_uTableSiz);
	}
	*/

	if (m_pMobTemplate->bIsTargetFromSrv)
		oPacket->Encode4(0);

	oPacket->Encode1(0); //Line 254
	if (GetTemplateID() / 10000 == 961) //Line 258
		oPacket->EncodeStr("");
	oPacket->Encode4(0); //Line 268
}

void Mob::SendChangeControllerPacket(User* pUser, int nLevel)
{
	if (nLevel)
	{
		OutPacket oPacket;
		oPacket.Encode2(MobSendPacketFlag::Mob_OnMobChangeController);
		oPacket.Encode1(nLevel);
		EncodeInitData(&oPacket, true);
		pUser->SendPacket(&oPacket);
	}
	else
		SendReleaseControllPacket(pUser, GetFieldObjectID());
}

void Mob::SendReleaseControllPacket(User* pUser, int dwMobID)
{
	OutPacket oPacket;
	oPacket.Encode2(MobSendPacketFlag::Mob_OnMobChangeController);
	oPacket.Encode1(0);
	oPacket.Encode4(dwMobID);
	//EncodeInitData(&oPacket);
	pUser->SendPacket(&oPacket);
}

void Mob::SetMobTemplate(MobTemplate *pTemplate)
{
	m_pMobTemplate = pTemplate;
}

const MobTemplate* Mob::GetMobTemplate() const
{
	return m_pMobTemplate;
}

void Mob::SetController(Controller * pController)
{
	m_pController = pController;
}

Controller* Mob::GetController()
{
	return m_pController;
}

void Mob::SetMovePosition(int x, int y, char bMoveAction, short nSN)
{
	//v6 = m_stat.nDoom_ == 0;
	SetPosX(x);
	SetPosY(y);
	SetMoveAction(bMoveAction);

	SetFh(nSN);
}

bool Mob::IsLucidSpecialMob(int dwTemplateID)
{
	if ((dwTemplateID == 8880150 || dwTemplateID == 8880151 || (dwTemplateID >= 8880160 && dwTemplateID < 8880200)) 
		&& (dwTemplateID != 8880182) 
		&& (dwTemplateID != 8880184) 
		&& (dwTemplateID != 8880171) 
		&& (dwTemplateID != 8880161)
		)
		return true;
	return false;
}

void Mob::OnMobHit(User * pUser, long long int nDamage, int nAttackType)
{
	m_mAttackRecord[pUser->GetUserID()] += nDamage;
	this->SetHP(this->GetHP() - nDamage);
	if (GetHP() > 0)
	{
		OutPacket oPacket;
		oPacket.Encode2(MobSendPacketFlag::Mob_OnHPIndicator);
		oPacket.Encode4(GetFieldObjectID());
		oPacket.Encode1((char)((GetHP() / GetMobTemplate()->m_lnMaxHP) * 100));
		pUser->SendPacket(&oPacket);
	}
}

void Mob::OnMobDead(int nHitX, int nHitY, int nMesoUp, int nMesoUpByItem)
{
	GiveReward(
		0,
		0,
		0,
		nHitX,
		nHitY,
		0,
		nMesoUp,
		nMesoUpByItem
	);
	int nOwnType, nOwnPartyID, nLastDamageCharacterID;
	DistributeExp(nOwnType, nOwnPartyID, nLastDamageCharacterID);
}

void Mob::DistributeExp(int & refOwnType, int & refOwnParyID, int & refLastDamageCharacterID)
{
	long long int nTotalHP = GetMobTemplate()->m_lnMaxHP;
	for (auto& dmg : m_mAttackRecord)
	{
		auto pUser = User::FindUser(dmg.first);
		if (pUser != nullptr)
		{
			auto nDamaged = dmg.second;
			if (nDamaged >= nTotalHP)
				nDamaged = nTotalHP;
			int nIncEXP = (int)floor((double)GetMobTemplate()->m_nEXP * ((double)dmg.second / (double)nTotalHP));
			auto nStatChanged = QWUser::IncEXP(pUser, nIncEXP, false);
			pUser->SendCharacterStat(false, nStatChanged);
		}
	}
}

void Mob::GiveReward(unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, int nX, int nY, int tDelay, int nMesoUp, int nMesoUpByItem)
{
	auto pReward = m_pMobTemplate->GetMobReward();
	if(!pReward)
		pReward = m_pMobTemplate->m_pReward = GW_MobReward::GetInstance()->GetMobReward(m_nTemplateID);

	int nDiff, nRange;
	bool bMoneyDropped = false;
	std::pair<int, int> prDropPos;

	const auto& aReward = m_pMobTemplate->GetMobReward()->GetRewardList();
	Reward* pDrop = nullptr;
	if (pReward) 
	{
		int nDropCount = 0;
		for (const auto& pInfo : aReward)
		{
			long long int liRnd = ((unsigned int)Rand32::GetInstance()->Random()) % pReward->GetTotalWeight();
			if (liRnd < pInfo->nWeight)
			{
				++nDropCount;
				prDropPos = { GetPosX(), GetPosY() };
				prDropPos.first = (prDropPos.first + ((nDropCount % 2 == 0) ? (25 * (nDropCount + 1) / 2) : -(25 * (nDropCount / 2))));
				nDiff = pInfo->nCountMax - pInfo->nCountMin;
				nRange = pInfo->nCountMin + (nDiff == 0 ? 0 : ((unsigned int)Rand32::GetInstance()->Random()) % nDiff);
				pDrop = AllocObj(Reward);
				if (pInfo->nItemID == 0)
					bMoneyDropped = true;
				pDrop->SetMoney(pInfo->nItemID == 0 ? nRange : 0);
				if (pInfo->nItemID != 0)
				{
					auto pItem = ItemInfo::GetInstance()->GetItemSlot(pInfo->nItemID, ItemInfo::ItemVariationOption::ITEMVARIATION_NORMAL);
					pDrop->SetItem(pItem);
					if (pInfo->nItemID / 1000000 != 1)
						((GW_ItemSlotBundle*)pItem)->nNumber = nRange;
				}
				pDrop->SetType(1);
				GetField()->GetDropPool()->Create(
					pDrop,
					dwOwnerID,
					dwOwnPartyID,
					nOwnType,
					GetTemplateID(),
					prDropPos.first,
					prDropPos.second,
					prDropPos.first,
					prDropPos.second,
					0,
					1,
					0,
					0);
			}
		}
	}

	if (!bMoneyDropped && ((unsigned int)(Rand32::GetInstance()->Random())) % 100 > 60)
	{
		prDropPos = GetDropPos();
		int nRange = (int)ceil((double)GetMobTemplate()->m_nLevel / 2.0) + ((unsigned int)(Rand32::GetInstance()->Random())) % (GetMobTemplate()->m_nLevel * 2);
		//printf("Drop Meso : Monster Level : %d Rnd : %d\n", (int)ceil((double)GetMobTemplate()->m_nLevel / 2.0), ((unsigned int)(Rand32::GetInstance()->Random())) % (GetMobTemplate()->m_nLevel * 2));
		pDrop = AllocObj(Reward);
		pDrop->SetItem(nullptr);
		pDrop->SetMoney(nRange);
		pDrop->SetType(0);
		GetField()->GetDropPool()->Create(
			pDrop,
			dwOwnerID,
			dwOwnPartyID,
			nOwnType,
			GetTemplateID(),
			prDropPos.first,
			prDropPos.second,
			prDropPos.first,
			prDropPos.second,
			0,
			1,
			0,
			0);
	}
}

void Mob::SetHP(long long int liHP)
{
	m_liHP = liHP;
}

void Mob::SetMP(long long int liMP)
{
	m_liMP = liMP;
}

long long int Mob::GetHP() const
{
	return m_liHP;
}

long long int Mob::GetMP() const
{
	return m_liMP;
}

std::pair<int, int> Mob::GetDropPos()
{
	std::pair<int, int> ret;

	int nPosDiff = 0, nDropPosX = GetPosX(), nDropPosY = GetPosY();

	nPosDiff = ((Rand32::GetInstance()->Random())) % 12;
	nDropPosX = (nDropPosX + ((nPosDiff % 2 == 0) ? (25 * (nPosDiff + 1) / 2) : -(nPosDiff * (nPosDiff / 2))));
	ret.first = nDropPosX;
	ret.second = nDropPosY;
	return ret;
}
