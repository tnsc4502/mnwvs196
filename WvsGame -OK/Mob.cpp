#include "Mob.h"
#include "MobTemplate.h"
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
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode1(1); //Control Mode
	oPacket->Encode4(GetTemplateID());

	//MobStat::EncodeForcedStat
	oPacket->Encode1(m_pMobTemplate->m_bIsChangeableMob);
	if (m_pMobTemplate->m_bIsChangeableMob)
	{
		oPacket->Encode4(m_pMobTemplate->m_lnMaxHP > INT_MAX ? INT_MAX : m_pMobTemplate->m_lnMaxHP);
		oPacket->Encode4(m_pMobTemplate->m_lnMaxMP > INT_MAX ? INT_MAX : m_pMobTemplate->m_lnMaxMP);
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
	//unsigned char* zeroBuff = (unsigned char*)stMemoryPoolMan->AllocateArray(12);
	///memset(zeroBuff, 0, 12);
	oPacket->EncodeBuffer(nullptr, 12);
	//stMemoryPoolMan->DestructArray(zerb)

	//MobStat::EncodeTemporary
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetPosY());
	oPacket->Encode1(GetFh()); // Stance? m_bMoveAction

	/*if (m_pMobTemplate->m_bIsSmartPhase)
		oPacket->Encode1(0);*/

	oPacket->Encode2(GetFh()); // Current FH m_nFootholdSN
	oPacket->Encode2(GetFh()); // Origin FH m_nHomeFoothold

	oPacket->Encode2(m_pMobTemplate->m_nSummonType);
	if (m_pMobTemplate->m_nSummonType == -3 || m_pMobTemplate->m_nSummonType >= 0)
		oPacket->Encode4(0); //dwSummonOption  -->  Linked ObjectID

	oPacket->Encode1(0); //Carnival Team
	oPacket->Encode4((int)m_pMobTemplate->m_lnMaxHP);
	
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	oPacket->Encode4(-1);
	oPacket->Encode4(-1);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0x64);
	oPacket->Encode4(-1);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode2(0);
}

void Mob::SetMobTemplate(MobTemplate *pTemplate)
{
	m_pMobTemplate = pTemplate;
}

const MobTemplate* Mob::GetMobTemplate() const
{
	return m_pMobTemplate;
}