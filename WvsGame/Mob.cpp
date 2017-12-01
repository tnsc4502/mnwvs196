#include "Mob.h"
#include "MobTemplate.h"
#include "User.h"
#include "Net\OutPacket.h"

Mob::Mob()
{
}

Mob::~Mob()
{
}

void Mob::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2(0x3C1); //MobPool::SpawnMonster
	oPacket->Encode1(0);
	EncodeInitData(oPacket);
}

void Mob::EncodeInitData(OutPacket *oPacket, bool bIsControl)
{
	printf("Encode Init Data oid = %d template ID = %d\n", GetFieldObjectID(), GetTemplateID());
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

	oPacket->Encode4(0); //m_nPhase
	oPacket->Encode4(0); //m_nCurZoneDataType
	oPacket->Encode4(0); //m_dwRefImgMobID

	//oPacket->Encode1(0);
	//m_dwLifeReleaseOwnerAID = CInPacket::Decode4
	//m_sLifeReleaseOwnerName = CInPacket::DecodeStr
	//m_sLifeReleaseMobName = CInPacket::DecodeStr

	oPacket->Encode4(0); //unk
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
		oPacket.Encode2(0x3C3);
		oPacket.Encode1(nLevel);
		EncodeInitData(&oPacket, true);
		pUser->SendPacket(&oPacket);
	}
	else
	{
		SendReleaseControllPacket(pUser, GetFieldObjectID());
	}
}

void Mob::SendReleaseControllPacket(User* pUser, int dwMobID)
{
	OutPacket oPacket;
	oPacket.Encode2(0x3C3);
	oPacket.Encode1(0);
	oPacket.Encode4(dwMobID);
	EncodeInitData(&oPacket);
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
