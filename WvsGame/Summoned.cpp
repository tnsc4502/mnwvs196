#include "Summoned.h"
#include "User.h"
#include "MovePath.h"
#include "Field.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterLevel.h"
#include "..\WvsLib\Net\PacketFlags\SummonedPacketFlags.hpp"

Summoned::Summoned()
{
}

Summoned::~Summoned()
{
}

int Summoned::GetOwnerID() const
{
	return m_pOwner->GetUserID();
}

int Summoned::GetSkillID() const
{
	return m_nSkillID;
}

void Summoned::Init(User * pUser, int nSkillID, int nSLV)
{
	m_pOwner = pUser;
	m_nSkillID = nSkillID;
	m_nSLV = nSLV;
	m_pField = pUser->GetField();

	m_nMoveAbility = GetMoveAbility();
	m_nAssitType = GetAssitType();
}

void Summoned::OnPacket(InPacket * iPacket)
{
	int nType = iPacket->Decode2();
	switch (nType)
	{
		case SummonedRecvPacketFlag::Summoned_OnMoveRequest:
			OnMove(iPacket);
			break;
		case SummonedRecvPacketFlag::Summoned_OnAttackRequest:
			break;
		case SummonedRecvPacketFlag::Summoned_OnHitRequest:
			break;
		case SummonedRecvPacketFlag::Summoned_OnDoingHealRequest:
			break;
		case SummonedRecvPacketFlag::Summoned_OnRemoveRequest:
			break;
		case SummonedRecvPacketFlag::Summoned_OnAttackForPvPRequest:
			break;
		case SummonedRecvPacketFlag::Summoned_OnActionRequest:
			break;
		case SummonedRecvPacketFlag::Summoned_OnAssistAttackRequest:
			break;
	}
}

void Summoned::OnMove(InPacket * iPacket)
{
	iPacket->Offset(12);
	MovePath movePath;
	movePath.Decode(iPacket);
	ValidateMovePath(&movePath);

	OutPacket oPacket;
	oPacket.Encode2((short)SummonedSendPacketFlag::Summoned_OnMove);
	oPacket.Encode4(m_pOwner->GetUserID());
	oPacket.Encode4(GetFieldObjectID());
	movePath.Encode(&oPacket);

	m_pField->BroadcastPacket(&oPacket);
}

void Summoned::MakeEnterFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2((short)SummonedSendPacketFlag::Summoned_OnCreated);
	oPacket->Encode4(m_pOwner->GetUserID());
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode4(36121002);
	oPacket->Encode1(m_pOwner->GetCharacterData()->mLevel->nLevel);
	oPacket->Encode1((unsigned char)(m_nSLV - 1));
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetPosY());
	oPacket->Encode1(GetMoveAction());
	oPacket->Encode2(GetFh());
	oPacket->Encode1(m_nMoveAbility);
	oPacket->Encode1(m_nAssitType);
	oPacket->Encode1(m_nEnterType);
	oPacket->Encode4(m_nMobID);
	oPacket->Encode1(m_bFlyMob);
	oPacket->Encode1(m_bBeforeFirstAttack);
	oPacket->Encode4(m_nLookID);
	oPacket->Encode4(m_nBulletID);

	oPacket->Encode1(m_bMirrored);
	if (m_bMirrored)
		m_pOwner->GetCharacterData()->EncodeAvatarLook(oPacket);

	oPacket->Encode1(m_bJaguarActive);
	oPacket->Encode4(m_tEnd);
	oPacket->Encode1(m_bAttackActive);
	oPacket->Encode4(0);
	oPacket->Encode1(1);
	oPacket->Encode4(0);
}

void Summoned::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2((short)SummonedSendPacketFlag::Summoned_OnRemoved);
	oPacket->Encode4(m_pOwner->GetUserID());
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode1(0x0A);
}

int Summoned::GetMoveAbility()
{
	return SUMMONED_MOV;
}

int Summoned::GetAssitType()
{
	return SUMMONED_ATT;
}

int Summoned::SUMMONED_MOV = 0;
int Summoned::SUMMONED_ATT = 1; 