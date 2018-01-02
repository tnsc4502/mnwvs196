#include "User.h"
#include "ClientSocket.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_Avatar.hpp"

#include "..\Common\Net\OutPacket.h"
#include "..\Common\Net\InPacket.h"
#include "..\Common\Net\PacketFlags\ClientPacketFlags.hpp"
#include "..\Common\Net\PacketFlags\UserPacketFlags.h"

#include "FieldMan.h"
#include "Portal.h"
#include "PortalMap.h"
#include "Field.h"
#include "QWUInventory.h"
#include "BasicStat.h"
#include "SecondaryStat.h"
#include "USkill.h"
#include "WvsGameConstants.hpp"
#include "AttackInfo.h"
#include "LifePool.h"
#include "SkillInfo.h"
#include "Drop.h"
#include "DropPool.h"
#include "Reward.h"
#include "ItemInfo.h"

void User::TryParsingDamageData(AttackInfo * pInfo, InPacket * iPacket)
{
	int nDamageMobCount = pInfo->GetDamagedMobCount();
	int nDamagedCountPerMob = pInfo->GetDamageCountPerMob();
	for (int i = 0; i < nDamageMobCount; ++i)
	{
		int nObjectID = iPacket->Decode4();
		auto& ref = pInfo->m_mDmgInfo[nObjectID];
		iPacket->Decode1();
		iPacket->Decode1();
		iPacket->Decode1();
		iPacket->Decode1();
		iPacket->Decode1();
		iPacket->Decode4();
		iPacket->Decode1();
		iPacket->Decode2();
		iPacket->Decode2();
		iPacket->Decode2();
		iPacket->Decode2();

		if (pInfo->m_nType == ClientPacketFlag::OnUserAttack_MagicAttack) 
		{
			iPacket->Decode1();
			if (pInfo->m_nSkillID == 80001835)
				iPacket->Decode1();
			else
				iPacket->Decode2();
		} 
		else
			iPacket->Decode2();

		for (int j = 0; j < nDamagedCountPerMob; ++j) 
		{
			long long int nDmg = iPacket->Decode8();
			printf("Monster %d Damage : %d\n", nObjectID, (int)nDmg);
			ref.push_back(nDmg);
		}
		iPacket->Decode4();
		iPacket->Decode2();
		iPacket->Decode8();
	}

	pInfo->m_nX = iPacket->Decode2();
	pInfo->m_nY = iPacket->Decode2();
}

AttackInfo * User::TryParsingMeleeAttack(int nType, InPacket * iPacket)
{
	AttackInfo* ret = new AttackInfo;
	ret->m_nType = nType;
	ret->m_bFieldKey = iPacket->Decode1();
	ret->m_bAttackInfoFlag = iPacket->Decode1();
	int nSkillID = ret->m_nSkillID = iPacket->Decode4();
	ret->m_nSLV = iPacket->Decode1();

	if (nType != ClientPacketFlag::OnUserAttack_AreaDot)
		iPacket->Decode1();

	ret->m_dwCRC = iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode2();
	iPacket->Decode4();

	if (WvsGameConstants::IsKeyDownSkill(nSkillID) || WvsGameConstants::IsSuperNovaSkill(nSkillID))
		ret->m_tKeyDown = iPacket->Decode4();

	if (WvsGameConstants::IsSpecialMeleeAttack(nSkillID))
		ret->m_pGrenade = iPacket->Decode4();

	if (WvsGameConstants::IsZeroSkill(nSkillID))
		iPacket->Decode1();

	if (nType != ClientPacketFlag::OnUserAttack_BodyAttack)
		iPacket->Decode1();

	iPacket->Decode1();

	ret->m_nDisplay = iPacket->Decode2();
	iPacket->Decode4();
	ret->m_nAttackActionType = iPacket->Decode1();
	ret->m_nAttackSpeed = iPacket->Decode1();

	if (nType != ClientPacketFlag::OnUserAttack_BodyAttack)
		ret->m_tLastAttackTime = iPacket->Decode4();

	iPacket->Decode4();
	ret->m_nFinalAttack = iPacket->Decode4();
	if (nSkillID > 0 && ret->m_nFinalAttack > 0)
		iPacket->Decode1();

	if (nSkillID == 5111009)
		iPacket->Decode1();

	if (WvsGameConstants::IsUsingBulletMeleeAttack(nSkillID))
	{
		iPacket->Decode2();
		if (nSkillID == 14000028 || nSkillID == 14000029 || nSkillID == 14121003 || nSkillID == 14121052)
			iPacket->Decode4();
	}

	if (nSkillID == 25111005)
		iPacket->Decode4();

	TryParsingDamageData(ret, iPacket);
	return ret;
}

AttackInfo * User::TryParsingMagicAttack(int nType, InPacket * iPacket)
{
	AttackInfo* ret = new AttackInfo;
	ret->m_nType = nType;
	ret->m_bFieldKey = iPacket->Decode1();
	ret->m_bAttackInfoFlag = iPacket->Decode1();
	int nSkillID = ret->m_nSkillID = iPacket->Decode4();
	ret->m_nSLV = iPacket->Decode1();

	ret->m_dwCRC = iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode2();
	iPacket->Decode4();

	if (WvsGameConstants::IsKeyDownSkill(nSkillID))
		ret->m_tKeyDown = iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode1();

	ret->m_nDisplay = iPacket->Decode2();
	iPacket->Decode4();
	ret->m_nAttackActionType = iPacket->Decode1();
	if (WvsGameConstants::IsEvanForceSkill(nSkillID))
		ret->m_bEvanForceAction = iPacket->Decode1();

	ret->m_nAttackSpeed = iPacket->Decode1();
	ret->m_tLastAttackTime = iPacket->Decode4();
	iPacket->Decode4();

	TryParsingDamageData(ret, iPacket);
	return ret;
}

AttackInfo * User::TryParsingShootAttack(int nType, InPacket * iPacket)
{
	AttackInfo* ret = new AttackInfo;
	iPacket->Decode1();
	ret->m_nType = nType;
	ret->m_bFieldKey = iPacket->Decode1();
	ret->m_bAttackInfoFlag = iPacket->Decode1();
	int nSkillID = ret->m_nSkillID = iPacket->Decode4();
	ret->m_nSLV = iPacket->Decode1();
	ret->m_bAddAttackProc = iPacket->Decode1();
	ret->m_dwCRC = iPacket->Decode4();

	iPacket->Decode1();
	ret->m_nSlot = iPacket->Decode2();
	ret->m_nCsStar = iPacket->Decode4();

	//int tKeyDown = 0, pGrenade;

	if (WvsGameConstants::IsKeyDownSkill(nSkillID))
		ret->m_tKeyDown = iPacket->Decode4();

	if (WvsGameConstants::IsZeroSkill(nSkillID))
		iPacket->Decode1();

	iPacket->Decode1();
	iPacket->Decode1();

	ret->m_apMinion = iPacket->Decode4();
	ret->m_bCheckExJablinResult = iPacket->Decode1();

	if (nSkillID == 3111013)
	{
		iPacket->Decode4();
		int nX = iPacket->Decode2();
		int nY = iPacket->Decode2();
	}

	ret->m_nDisplay = iPacket->Decode2();
	iPacket->Decode4();
	ret->m_nAttackActionType = iPacket->Decode1();

	if (nSkillID == 23111001 || nSkillID == 80001915 || nSkillID == 36111010)
	{
		iPacket->Decode4();
		iPacket->Decode4();
		iPacket->Decode4();
	}

	ret->m_nAttackSpeed = iPacket->Decode1();
	ret->m_tLastAttackTime = iPacket->Decode4();
	ret->m_nSoulArrow = iPacket->Decode4();
	ret->m_nWeaponType = iPacket->Decode2();
	ret->m_nShootRange = iPacket->Decode1();

	iPacket->Decode2(); //unk
	iPacket->Decode2(); //unk
	iPacket->Decode2(); //unk
	iPacket->Decode2(); //unk

	TryParsingDamageData(ret, iPacket);
	return ret;
}

AttackInfo * User::TryParsingAreaDot(int nType, InPacket * iPacket)
{
	return TryParsingMeleeAttack(nType, iPacket);
}

AttackInfo * User::TryParsingBodyAttack(int nType, InPacket * iPacket)
{
	return TryParsingMeleeAttack(nType, iPacket);
}

User::User(ClientSocket *_pSocket, InPacket *iPacket)
	: pSocket(_pSocket),
	  pCharacterData(new GA_Character()),
	  m_pBasicStat(new BasicStat),
	  m_pSecondaryStat(new SecondaryStat)
{
	pCharacterData->DecodeCharacterData(iPacket);
	_pSocket->SetUser(this);
	pField = (FieldMan::GetInstance()->GetField(pCharacterData->nFieldID));
	pField->OnEnter(this);
}

User::~User()
{
	//pField->OnLeave(this);
	LeaveField();
	
	delete pCharacterData;
	delete m_pBasicStat;
	delete m_pSecondaryStat;
}

int User::GetUserID() const
{
	return pCharacterData->nCharacterID;
}

void User::SendPacket(OutPacket *oPacket)
{
	pSocket->SendPacket(oPacket);
}

GA_Character * User::GetCharacterData()
{
	return pCharacterData;
}

Field * User::GetField()
{
	return pField;
}

void User::MakeEnterFieldPacket(OutPacket *oPacket)
{

}

void User::OnPacket(InPacket *iPacket)
{
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case ClientPacketFlag::OnUserChat:
		OnChat(iPacket);
		break;
	case ClientPacketFlag::OnUserTransferFieldRequest:
		OnTransferFieldRequest(iPacket);
		break;
	case ClientPacketFlag::OnUserMoveRequest:
		pField->OnUserMove(this, iPacket);
		break;
	case ClientPacketFlag::OnUserChangeSlotRequest:
		QWUInventory::OnChangeSlotPositionRequest(this, iPacket);
		break;
	case ClientPacketFlag::OnUserSkillUpRequest:
		USkill::OnSkillUpRequest(this, iPacket);
		break;
	case ClientPacketFlag::OnUserSkillUseRequest:
		USkill::OnSkillUseRequest(this, iPacket);
		break;
	case ClientPacketFlag::OnUserAttack_MeleeAttack:
	case ClientPacketFlag::OnUserAttack_ShootAttack:
	case ClientPacketFlag::OnUserAttack_MagicAttack:
	case ClientPacketFlag::OnUserAttack_BodyAttack:
	case ClientPacketFlag::OnUserAttack_AreaDot:
		OnAttack(nType, iPacket);
		break;
	default:
		if (pField)
		{
			iPacket->RestorePacket();
			pField->OnPacket(this, iPacket);
		}
	}
}

void User::OnTransferFieldRequest(InPacket * iPacket)
{
	if (!pField)
		pSocket->GetSocket().close();
	iPacket->Decode1(); //ms_RTTI_CField ?
	int dwFieldReturn = iPacket->Decode4();
	std::string sPortalName = iPacket->DecodeStr();
	if (sPortalName.size() > 0)
	{
		iPacket->Decode2(); //not sure
		iPacket->Decode2(); //not sure
	}
	std::lock_guard<std::mutex> user_lock(m_mtxUserlock);
	/*
	if(m_character.characterStat.nHP == 0)
	{
		m_basicStat->SetFrom(m_character, m_aRealEquip, m_aRealEqup2, 0, 0, 0);
		m_secondaryStat->Clear();
		....
	}
	*/
	Portal* pPortal = pField->GetPortalMap()->FindPortal(sPortalName);
	Field *pTargetField = FieldMan::GetInstance()->GetField(dwFieldReturn == -1 ? pPortal->GetTargetMap() : dwFieldReturn);
	if (pTargetField != nullptr)
	{
		LeaveField();
		pField = pTargetField;
		PostTransferField(pField->GetFieldID(), pPortal, false);
		pField->OnEnter(this);
	}
}

void User::OnChat(InPacket *iPacket)
{
	iPacket->Decode4(); //TIME TICK
	std::string strMsg = iPacket->DecodeStr();
	unsigned char balloon = iPacket->Decode1();

	OutPacket oPacket;
	oPacket.Encode2(UserPacketFlag::UserCommon_OnChat);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(0);
	oPacket.EncodeStr(strMsg);
	oPacket.Encode1(balloon);
	oPacket.Encode1(0);
	oPacket.Encode1(-1);

	pField->SplitSendPacket(&oPacket, nullptr);
}

void User::PostTransferField(int dwFieldID, Portal * pPortal, int bForce)
{
	OutPacket oPacket;
	oPacket.Encode2(0x1BF); //Set Stage
	oPacket.Encode4(0); //nChannel
	oPacket.Encode1(0);
	oPacket.Encode4(0);

	oPacket.Encode1(2); //bCharacterData?
	oPacket.Encode4(0);
	oPacket.Encode4(1024);
	oPacket.Encode4(768);

	oPacket.Encode1(0); // Change Stage(1) or Transfer Field(0)
	oPacket.Encode2(0);

	oPacket.Encode1(0); //bUsingBuffProtector
	oPacket.Encode4(dwFieldID);
	oPacket.Encode1(0);
	oPacket.Encode4(pCharacterData->mStat->nHP); //HP

	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);

	oPacket.Encode8(std::time(nullptr));
	oPacket.EncodeHexString("64 00 00 00 00 00 00 01 A6 00 00 00 03 00 00 00 83 7D 26 5A 02 00 00 24 66 00 00 00 00 00 03 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 40 E0 FD 3B 37 4F 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 82 16 FB 52 01 00 00 24 0C 00 00 00 00 00 00 00 00 00 00 00 C8 00 00 00 F7 24 11 76 00 00 00 24 0C 00 00 00 01 00 00 24 02 00 00 24 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 96 00 00 00 00");
	SendPacket(&oPacket);
}

void User::SetMovePosition(int x, int y, char bMoveAction, short nFSN)
{
	SetPosX(x);
	SetPosY(y);
	SetMoveAction(bMoveAction);
	SetFh(nFSN);
}

void User::OnAvatarModified()
{
	OutPacket oPacket;
	oPacket.Encode2(UserPacketFlag::UserRemote_OnAvatarModified);
	oPacket.Encode4(nCharacterID);
	int dwAvatarModFlag = 1;
	oPacket.Encode1(dwAvatarModFlag); //m_dwAvatarModFlag
	if (dwAvatarModFlag & 1)
		this->pCharacterData->EncodeAvatarLook(&oPacket);
	if (dwAvatarModFlag & 2)
		oPacket.Encode1(0); //secondayStat.nSpeed
	if (dwAvatarModFlag & 4)
		oPacket.Encode1(0); //nChoco

	EncodeCoupleInfo(&oPacket);
	EncodeFriendshipInfo(&oPacket);
	EncodeMarriageInfo(&oPacket);

	oPacket.Encode4(0);
	oPacket.Encode4(0);
	oPacket.Encode4(0);

	pField->BroadcastPacket(&oPacket);
}

void User::EncodeCoupleInfo(OutPacket * oPacket)
{
	oPacket->Encode1(0);
	for (int i = 0; i < 0; ++i)
	{
		oPacket->Encode4(1);
		oPacket->Encode8(0); //liSN
		oPacket->Encode8(0); //liPairSN
		oPacket->Encode4(0); //nItemID
	}
}

void User::EncodeFriendshipInfo(OutPacket * oPacket)
{
	oPacket->Encode1(0);
	for (int i = 0; i < 0; ++i)
	{
		oPacket->Encode4(1);
		oPacket->Encode8(0); //liSN
		oPacket->Encode8(0); //liPairSN
		oPacket->Encode4(0); //nItemID
	}
}

void User::EncodeMarriageInfo(OutPacket * oPacket)
{
	oPacket->Encode1(0);
	for (int i = 0; i < 0; ++i)
	{
		oPacket->Encode4(nCharacterID); //
		oPacket->Encode4(0); //nPairID
		oPacket->Encode4(0); //nItemID
	}
}

void User::ValidateStat()
{
}

void User::SendCharacterStat(bool bOnExclRequest, long long int liFlag)
{
	OutPacket oPacket;
	oPacket.Encode2(UserPacketFlag::UserLocal_OnStatChanged);
	oPacket.Encode1((char)bOnExclRequest);
	oPacket.Encode8(liFlag);
	if (liFlag & BasicStat::BasicStatFlag::BS_Skin)
		oPacket.Encode1(pCharacterData->mAvatarData->nSkin);
	if (liFlag & BasicStat::BasicStatFlag::BS_Face)
		oPacket.Encode4(pCharacterData->mAvatarData->nFace);
	if (liFlag & BasicStat::BasicStatFlag::BS_Hair)
		oPacket.Encode4(pCharacterData->mAvatarData->nHair);
	if (liFlag & BasicStat::BasicStatFlag::BS_Level)
		oPacket.Encode1(pCharacterData->mLevel->nLevel);
	if (liFlag & BasicStat::BasicStatFlag::BS_Job)
	{
		oPacket.Encode2(pCharacterData->mStat->nJob);
		oPacket.Encode2(pCharacterData->mStat->nSubJob);
	}

	if (liFlag & BasicStat::BasicStatFlag::BS_STR)
		oPacket.Encode2(pCharacterData->mStat->nStr);
	if (liFlag & BasicStat::BasicStatFlag::BS_DEX)
		oPacket.Encode2(pCharacterData->mStat->nDex);
	if (liFlag & BasicStat::BasicStatFlag::BS_INT)
		oPacket.Encode2(pCharacterData->mStat->nInt);
	if (liFlag & BasicStat::BasicStatFlag::BS_LUK)
		oPacket.Encode2(pCharacterData->mStat->nLuk);
	if (liFlag & BasicStat::BasicStatFlag::BS_HP)
		oPacket.Encode4(pCharacterData->mStat->nHP);
	if (liFlag & BasicStat::BasicStatFlag::BS_MaxHP)
		oPacket.Encode4(pCharacterData->mStat->nMaxHP);
	if (liFlag & BasicStat::BasicStatFlag::BS_MP)
		oPacket.Encode4(pCharacterData->mStat->nMP);
	if (liFlag & BasicStat::BasicStatFlag::BS_MaxMP)
		oPacket.Encode4(pCharacterData->mStat->nMaxMP);

	if (liFlag & BasicStat::BasicStatFlag::BS_AP)
		oPacket.Encode2(pCharacterData->mStat->nAP);

	//not done yet.
	if (liFlag & BasicStat::BasicStatFlag::BS_SP)
		oPacket.Encode2(10);

	if (liFlag & BasicStat::BasicStatFlag::BS_EXP)
		oPacket.Encode8(pCharacterData->mStat->nExp);
	if (liFlag & BasicStat::BasicStatFlag::BS_POP)
		oPacket.Encode4(pCharacterData->mStat->nPOP);
	if (liFlag & BasicStat::BasicStatFlag::BS_Meso)
		oPacket.Encode8(pCharacterData->mMoney->nMoney);

	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);

	SendPacket(&oPacket);
}

void User::SendTemporaryStatReset(long long int uFlag)
{
}

void User::SendTemporaryStatSet(TemporaryStat::TS_Flag& flag, int tDelay)
{
	OutPacket oPacket;
	oPacket.Encode2(UserPacketFlag::USerLocal_OnTemporaryStatSet);
	m_pSecondaryStat->EncodeForLocal(&oPacket, flag);
	SendPacket(&oPacket);
}

void User::OnAttack(int nType, InPacket * iPacket)
{
	std::unique_ptr<AttackInfo> pInfo = nullptr;
	switch (nType)
	{
	case ClientPacketFlag::OnUserAttack_MeleeAttack:
		pInfo.reset(TryParsingMeleeAttack(nType, iPacket));
		break;
	case ClientPacketFlag::OnUserAttack_ShootAttack:
		pInfo.reset(TryParsingShootAttack(nType, iPacket));
		break;
	case ClientPacketFlag::OnUserAttack_MagicAttack:
		pInfo.reset(TryParsingMagicAttack(nType, iPacket));
		break;
	case ClientPacketFlag::OnUserAttack_BodyAttack:
		pInfo.reset(TryParsingBodyAttack(nType, iPacket));
		break;
	case ClientPacketFlag::OnUserAttack_AreaDot:
		pInfo.reset(TryParsingAreaDot(nType, iPacket));
		break;
	}
	if (pInfo) 
	{
		pField->GetLifePool()->OnUserAttack(
			this,
			SkillInfo::GetInstance()->GetSkillByID(pInfo->m_nSkillID),
			pInfo.get()
		);
	}
}

SecondaryStat * User::GetSecondaryStat()
{
	return this->m_pSecondaryStat;
}

BasicStat * User::GetBasicStat()
{
	return this->m_pBasicStat;
}

void User::LeaveField()
{
	pField->OnLeave(this);
}