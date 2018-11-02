#include "User.h"
#include "ClientSocket.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_Avatar.hpp"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\PacketFlags\GamePacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"

#include "FieldMan.h"
#include "Portal.h"
#include "PortalMap.h"
#include "WvsGame.h"
#include "Field.h"
#include "QWUser.h"
#include "QWUInventory.h"
#include "BasicStat.h"
#include "SecondaryStat.h"
#include "USkill.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "AttackInfo.h"
#include "NpcTemplate.h"
#include "LifePool.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "CommandManager.h"
#include "QuestMan.h"
#include "QuestAct.h"
#include "ActItem.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Random\Rand32.h"

#include "QWUQuestRecord.h"
#include "InventoryManipulator.h"
#include "ScriptMan.h"
#include "Script.h"

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

		if (pInfo->m_nType == UserRecvPacketFlag::User_OnUserAttack_MagicAttack) 
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
			//printf("Monster %d Damage : %d\n", nObjectID, (int)nDmg);
			ref.push_back(nDmg);
		}
		iPacket->Decode4();
		iPacket->Decode2();
		iPacket->Decode8();
	}

	pInfo->m_nX = iPacket->Decode2();
	pInfo->m_nY = iPacket->Decode2();
}

AttackInfo * User::TryParsingMeleeAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bAttackInfoFlag = iPacket->Decode1();
	int nSkillID = pInfo->m_nSkillID = iPacket->Decode4();
	pInfo->m_nSLV = iPacket->Decode1();

	if (nType != UserRecvPacketFlag::User_OnUserAttack_AreaDot)
		iPacket->Decode1();

	pInfo->m_dwCRC = iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode2();
	iPacket->Decode4();

	if (WvsGameConstants::IsKeyDownSkill(nSkillID) || WvsGameConstants::IsSuperNovaSkill(nSkillID))
		pInfo->m_tKeyDown = iPacket->Decode4();

	if (WvsGameConstants::IsSpecialMeleeAttack(nSkillID))
		pInfo->m_pGrenade = iPacket->Decode4();

	if (WvsGameConstants::IsZeroSkill(nSkillID))
		iPacket->Decode1();

	if (nType != UserRecvPacketFlag::User_OnUserAttack_BodyAttack)
		iPacket->Decode1();

	iPacket->Decode1();

	pInfo->m_nDisplay = iPacket->Decode2();
	iPacket->Decode4();
	pInfo->m_nAttackActionType = iPacket->Decode1();
	pInfo->m_nAttackSpeed = iPacket->Decode1();

	if (nType != UserRecvPacketFlag::User_OnUserAttack_BodyAttack)
		pInfo->m_tLastAttackTime = iPacket->Decode4();

	iPacket->Decode4();
	pInfo->m_nFinalAttack = iPacket->Decode4();
	if (nSkillID > 0 && pInfo->m_nFinalAttack > 0)
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

	TryParsingDamageData(pInfo, iPacket);
	return pInfo;
}

AttackInfo * User::TryParsingMagicAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bAttackInfoFlag = iPacket->Decode1();
	int nSkillID = pInfo->m_nSkillID = iPacket->Decode4();
	pInfo->m_nSLV = iPacket->Decode1();

	pInfo->m_dwCRC = iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode2();
	iPacket->Decode4();

	if (WvsGameConstants::IsKeyDownSkill(nSkillID))
		pInfo->m_tKeyDown = iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode1();

	pInfo->m_nDisplay = iPacket->Decode2();
	iPacket->Decode4();
	pInfo->m_nAttackActionType = iPacket->Decode1();
	if (WvsGameConstants::IsEvanForceSkill(nSkillID))
		pInfo->m_bEvanForceAction = iPacket->Decode1();

	pInfo->m_nAttackSpeed = iPacket->Decode1();
	pInfo->m_tLastAttackTime = iPacket->Decode4();
	iPacket->Decode4();

	TryParsingDamageData(pInfo, iPacket);
	return pInfo;
}

AttackInfo * User::TryParsingShootAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	iPacket->Decode1();
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bAttackInfoFlag = iPacket->Decode1();
	int nSkillID = pInfo->m_nSkillID = iPacket->Decode4();
	pInfo->m_nSLV = iPacket->Decode1();
	pInfo->m_bAddAttackProc = iPacket->Decode1();
	pInfo->m_dwCRC = iPacket->Decode4();

	iPacket->Decode1();
	pInfo->m_nSlot = iPacket->Decode2();
	pInfo->m_nCsStar = iPacket->Decode4();

	//int tKeyDown = 0, pGrenade;

	if (WvsGameConstants::IsKeyDownSkill(nSkillID))
		pInfo->m_tKeyDown = iPacket->Decode4();

	if (WvsGameConstants::IsZeroSkill(nSkillID))
		iPacket->Decode1();

	iPacket->Decode1();
	iPacket->Decode1();

	pInfo->m_apMinion = iPacket->Decode4();
	pInfo->m_bCheckExJablinResult = iPacket->Decode1();

	if (nSkillID == 3111013)
	{
		iPacket->Decode4();
		int nX = iPacket->Decode2();
		int nY = iPacket->Decode2();
	}

	pInfo->m_nDisplay = iPacket->Decode2();
	iPacket->Decode4();
	pInfo->m_nAttackActionType = iPacket->Decode1();

	if (nSkillID == 23111001 || nSkillID == 80001915 || nSkillID == 36111010)
	{
		iPacket->Decode4();
		iPacket->Decode4();
		iPacket->Decode4();
	}

	pInfo->m_nAttackSpeed = iPacket->Decode1();
	pInfo->m_tLastAttackTime = iPacket->Decode4();
	pInfo->m_nSoulArrow = iPacket->Decode4();
	pInfo->m_nWeaponType = iPacket->Decode2();
	pInfo->m_nShootRange = iPacket->Decode1();

	iPacket->Decode2(); //unk
	iPacket->Decode2(); //unk
	iPacket->Decode2(); //unk
	iPacket->Decode2(); //unk

	TryParsingDamageData(pInfo, iPacket);
	return pInfo;
}

AttackInfo * User::TryParsingAreaDot(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	return TryParsingMeleeAttack(pInfo, nType, iPacket);
}

AttackInfo * User::TryParsingBodyAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	return TryParsingMeleeAttack(pInfo, nType, iPacket);
}

void User::OnIssueReloginCookie(InPacket * iPacket)
{
	OnMigrateOut();
}

User::User(ClientSocket *_pSocket, InPacket *iPacket)
	: m_pSocket(_pSocket),
	m_pCharacterData(AllocObj(GA_Character)),
	  m_pBasicStat(AllocObj(BasicStat)),
	  m_pSecondaryStat(AllocObj(SecondaryStat))
{
	_pSocket->SetUser(this);
	m_pCharacterData->DecodeCharacterData(iPacket, true);
	m_pSecondaryStat->DecodeInternal(this, iPacket);
}

User::~User()
{
	OutPacket oPacket;
	oPacket.Encode2(GameSendPacketFlag::RequestMigrateOut);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	m_pCharacterData->EncodeCharacterData(&oPacket, true);
	if (m_nTransferStatus == TransferStatus::eOnTransferShop || m_nTransferStatus == TransferStatus::eOnTransferChannel) 
	{
		oPacket.Encode1(0); //bGameEnd
		m_pSecondaryStat->EncodeInternal(this, &oPacket);
	}
	else
		oPacket.Encode1(1); //bGameEnd, Dont decode and save the secondarystat info.
	WvsGame::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);

	auto bindT = std::bind(&User::Update, this);
	m_pUpdateTimer->Abort();
	//m_pField->OnLeave(this);
	LeaveField();
	
	try {
		if (GetScript())
			GetScript()->Abort();
	}
	catch (...) {}

	FreeObj(m_pCharacterData);
	FreeObj(m_pBasicStat);
	FreeObj(m_pSecondaryStat);
}

int User::GetUserID() const
{
	return m_pCharacterData->nCharacterID;
}

int User::GetChannelID() const
{
	return WvsBase::GetInstance<WvsGame>()->GetChannelID();
}

void User::SendPacket(OutPacket *oPacket)
{
	m_pSocket->SendPacket(oPacket);
}

GA_Character * User::GetCharacterData()
{
	return m_pCharacterData;
}

Field * User::GetField()
{
	return m_pField;
}

void User::MakeEnterFieldPacket(OutPacket *oPacket)
{

}

void User::MakeLeaveFieldPacket(OutPacket * oPacket)
{
}

void User::OnPacket(InPacket *iPacket)
{
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case UserRecvPacketFlag::User_OnStatChangeItemUseRequest:
		OnStatChangeItemUseRequest(iPacket, false);
		break;
	case UserRecvPacketFlag::User_OnStatChangeItemCancelRequest:
		OnStatChangeItemCancelRequest(iPacket);
		break;
	case 0x16B:
	{
		std::string strSkill = iPacket->DecodeStr();
		int nLVL1 = iPacket->Decode4();
		int nLVL2 = iPacket->Decode4();
		OutPacket oPacket;
		oPacket.Encode2(0xC1);
		oPacket.EncodeStr(strSkill);
		oPacket.Encode4(nLVL1);
		oPacket.Encode4(nLVL2);
		oPacket.Encode1(0);
		oPacket.Encode4(0);
		SendPacket(&oPacket);
		break;
	}
	case UserRecvPacketFlag::User_OnUserChat:
		OnChat(iPacket);
		break;
	case UserRecvPacketFlag::User_OnUserTransferFieldRequest:
		OnTransferFieldRequest(iPacket);
		break;
	case UserRecvPacketFlag::User_OnUserTransferChannelRequest:
		OnTransferChannelRequest(iPacket);
		break;
	case UserRecvPacketFlag::User_OnUserMigrateToCashShopRequest:
		OnMigrateToCashShopRequest(iPacket);
		break;
	case UserRecvPacketFlag::User_OnUserMoveRequest:
		m_pField->OnUserMove(this, iPacket);
		break;
	case UserRecvPacketFlag::User_OnUserChangeSlotRequest:
		QWUInventory::OnChangeSlotPositionRequest(this, iPacket);
		break;
	case UserRecvPacketFlag::User_OnUserSkillUpRequest:
		USkill::OnSkillUpRequest(this, iPacket);
		break;
	case UserRecvPacketFlag::User_OnUserSkillUseRequest:
		USkill::OnSkillUseRequest(this, iPacket);
		break;
	case UserRecvPacketFlag::User_OnUserSkillCancelRequest:
		USkill::OnSkillCancelRequest(this, iPacket);
		break;
	case UserRecvPacketFlag::User_OnUserAttack_MeleeAttack:
	case UserRecvPacketFlag::User_OnUserAttack_ShootAttack:
	case UserRecvPacketFlag::User_OnUserAttack_MagicAttack:
	case UserRecvPacketFlag::User_OnUserAttack_BodyAttack:
	case UserRecvPacketFlag::User_OnUserAttack_AreaDot:
		OnAttack(nType, iPacket);
		break;
	case UserRecvPacketFlag::User_OnChangeCharacterRequest:
		OnIssueReloginCookie(iPacket);
		break;
	case UserRecvPacketFlag::User_OnSelectNpc:
		OnSelectNpc(iPacket);
		break;
	case UserRecvPacketFlag::User_OnScriptMessageAnswer:
		OnScriptMessageAnswer(iPacket);
		break;
	case UserRecvPacketFlag::User_OnQuestRequest:
		OnQuestRequest(iPacket);
		break;
	default:
		if (m_pField)
		{
			iPacket->RestorePacket();
			m_pField->OnPacket(this, iPacket);
		}
	}
	ValidateStat();
	SendCharacterStat(false, 0);
}

void User::OnTransferFieldRequest(InPacket * iPacket)
{
	if (!m_pField)
		m_pSocket->GetSocket().close();
	iPacket->Decode1(); //ms_RTTI_CField ?
	int dwFieldReturn = iPacket->Decode4();
	std::string sPortalName = iPacket->DecodeStr();
	if (sPortalName.size() > 0)
	{
		iPacket->Decode2(); //not sure
		iPacket->Decode2(); //not sure
	}
	/*
	if(m_character.characterStat.nHP == 0)
	{
		m_basicStat->SetFrom(m_character, m_aRealEquip, m_aRealEqup2, 0, 0, 0);
		m_secondaryStat->Clear();
		....
	}
	*/
	TryTransferField(dwFieldReturn, sPortalName);
}

bool User::TryTransferField(int nFieldID, const std::string& sPortalName)
{
	std::lock_guard<std::mutex> user_lock(m_mtxUserlock);
	SetTransferStatus(TransferStatus::eOnTransferField);
	Portal* pPortal = m_pField->GetPortalMap()->FindPortal(sPortalName);
	Field *pTargetField = FieldMan::GetInstance()->GetField(
		nFieldID == -1 ?
		pPortal->GetTargetMap() :
		nFieldID
	);
	if (pTargetField != nullptr)
	{
		Portal* pTargetPortal = pPortal == nullptr ? nullptr : pTargetField->GetPortalMap()->FindPortal(pPortal->GetTargetPortalName());
		LeaveField();
		m_pField = pTargetField;
		PostTransferField(m_pField->GetFieldID(), pTargetPortal, false);
		m_pField->OnEnter(this);
		m_pCharacterData->nFieldID = m_pField->GetFieldID();
		SetTransferStatus(TransferStatus::eOnTransferNone);
		return true;
	}
	return false;
}

void User::OnTransferChannelRequest(InPacket * iPacket)
{
	int nChannelID = iPacket->Decode1();

	if (nChannelID == WvsBase::GetInstance<WvsGame>()->GetChannelID())
	{
		//SendTransferChannelIgnored
	}
	//Check if the server is connected.
	//Check if the user can attach additional process.

	SetTransferStatus(TransferStatus::eOnTransferChannel);
	OutPacket oPacket;
	oPacket.Encode2(GameSendPacketFlag::RequestTransferChannel);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(m_nCharacterID);
	oPacket.Encode1(nChannelID);
	WvsGame::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnMigrateToCashShopRequest(InPacket * iPacket)
{
	//Check if the server is connected.
	//Check if the user can attach additional process.
	SetTransferStatus(TransferStatus::eOnTransferShop);
	OutPacket oPacket;
	oPacket.Encode2(GameSendPacketFlag::RequestTransferShop);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(m_nCharacterID);
	WvsGame::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnChat(InPacket *iPacket)
{
	iPacket->Decode4(); //TIME TICK
	std::string strMsg = iPacket->DecodeStr();
	CommandManager::GetInstance()->Process(this, strMsg);

	unsigned char balloon = iPacket->Decode1();

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserCommon_OnChat);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(0);
	oPacket.EncodeStr(strMsg);
	oPacket.Encode1(balloon);
	oPacket.Encode1(0);
	oPacket.Encode1(-1);

	m_pField->SplitSendPacket(&oPacket, nullptr);
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
	oPacket.Encode1(pPortal == nullptr ? 0x80 : pPortal->GetID());
	oPacket.Encode4(m_pCharacterData->mStat->nHP); //HP

	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);

	oPacket.Encode8(GameDateTime::GetCurrentDate());
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
	oPacket.Encode2(UserSendPacketFlag::UserRemote_OnAvatarModified);
	oPacket.Encode4(m_nCharacterID);
	int dwAvatarModFlag = 1;
	oPacket.Encode1(dwAvatarModFlag); //m_dwAvatarModFlag
	if (dwAvatarModFlag & 1)
		this->m_pCharacterData->EncodeAvatarLook(&oPacket);
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

	m_pField->BroadcastPacket(&oPacket);
}

void User::EncodeCharacterData(OutPacket * oPacket)
{
	m_pCharacterData->EncodeCharacterData(oPacket, false);
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
		oPacket->Encode4(m_nCharacterID); //
		oPacket->Encode4(0); //nPairID
		oPacket->Encode4(0); //nItemID
	}
}

void User::ValidateStat(bool bCalledByConstructor)
{
	m_pBasicStat->SetFrom(m_pCharacterData, m_pSecondaryStat->nMaxHP, m_pSecondaryStat->nMaxMP, m_pSecondaryStat->nBasicStatUp);
	m_pSecondaryStat->SetFrom(m_pCharacterData, m_pBasicStat);
	long long int liFlag = 0;
	if (m_pCharacterData->mStat->nHP > m_pBasicStat->nMHP)
		liFlag |= QWUser::IncHP(this, 0, false);
	if (m_pCharacterData->mStat->nMP > m_pBasicStat->nMMP)
		liFlag |= QWUser::IncMP(this, 0, false);
	if (!bCalledByConstructor)
	{
		if (liFlag)
			SendCharacterStat(false, liFlag);
	}
}

void User::SendCharacterStat(bool bOnExclRequest, long long int liFlag)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnStatChanged);
	oPacket.Encode1((char)bOnExclRequest);
	oPacket.Encode8(liFlag);
	if (liFlag & BasicStat::BasicStatFlag::BS_Skin)
		oPacket.Encode1(m_pCharacterData->mAvatarData->nSkin);
	if (liFlag & BasicStat::BasicStatFlag::BS_Face)
		oPacket.Encode4(m_pCharacterData->mAvatarData->nFace);
	if (liFlag & BasicStat::BasicStatFlag::BS_Hair)
		oPacket.Encode4(m_pCharacterData->mAvatarData->nHair);
	if (liFlag & BasicStat::BasicStatFlag::BS_Level)
		oPacket.Encode1(m_pCharacterData->mLevel->nLevel);
	if (liFlag & BasicStat::BasicStatFlag::BS_Job)
	{
		oPacket.Encode2(m_pCharacterData->mStat->nJob);
		oPacket.Encode2(m_pCharacterData->mStat->nSubJob);
	}

	if (liFlag & BasicStat::BasicStatFlag::BS_STR)
		oPacket.Encode2(m_pCharacterData->mStat->nStr);
	if (liFlag & BasicStat::BasicStatFlag::BS_DEX)
		oPacket.Encode2(m_pCharacterData->mStat->nDex);
	if (liFlag & BasicStat::BasicStatFlag::BS_INT)
		oPacket.Encode2(m_pCharacterData->mStat->nInt);
	if (liFlag & BasicStat::BasicStatFlag::BS_LUK)
		oPacket.Encode2(m_pCharacterData->mStat->nLuk);
	if (liFlag & BasicStat::BasicStatFlag::BS_HP)
		oPacket.Encode4(m_pCharacterData->mStat->nHP);
	if (liFlag & BasicStat::BasicStatFlag::BS_MaxHP)
		oPacket.Encode4(m_pCharacterData->mStat->nMaxHP);
	if (liFlag & BasicStat::BasicStatFlag::BS_MP)
		oPacket.Encode4(m_pCharacterData->mStat->nMP);
	if (liFlag & BasicStat::BasicStatFlag::BS_MaxMP)
		oPacket.Encode4(m_pCharacterData->mStat->nMaxMP);

	if (liFlag & BasicStat::BasicStatFlag::BS_AP)
		oPacket.Encode2(m_pCharacterData->mStat->nAP);

	//not done yet.
	if (liFlag & BasicStat::BasicStatFlag::BS_SP)
		m_pCharacterData->mStat->EncodeExtendSP(&oPacket);

	if (liFlag & BasicStat::BasicStatFlag::BS_EXP)
		oPacket.Encode8(m_pCharacterData->mStat->nExp);
	if (liFlag & BasicStat::BasicStatFlag::BS_POP)
		oPacket.Encode4(m_pCharacterData->mStat->nPOP);
	if (liFlag & BasicStat::BasicStatFlag::BS_Meso)
		oPacket.Encode8(m_pCharacterData->mMoney->nMoney);

	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);

	SendPacket(&oPacket);
}

void User::SendTemporaryStatReset(TemporaryStat::TS_Flag& flag)
{
	if (flag.IsEmpty())
		return;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnTemporaryStatReset);
	flag.Encode(&oPacket);
	m_pSecondaryStat->EncodeIndieTempStat(&oPacket, flag);
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	SendPacket(&oPacket);
}

void User::SendTemporaryStatSet(TemporaryStat::TS_Flag& flag, int tDelay)
{
	if (flag.IsEmpty())
		return;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnTemporaryStatSet);
	m_pSecondaryStat->EncodeForLocal(&oPacket, flag);
	SendPacket(&oPacket);
}

void User::OnAttack(int nType, InPacket * iPacket)
{
	AttackInfo attackInfo, *pResult = nullptr;

	switch (nType)
	{
		case UserRecvPacketFlag::User_OnUserAttack_MeleeAttack:
			pResult = (TryParsingMeleeAttack(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketFlag::User_OnUserAttack_ShootAttack:
			pResult = (TryParsingShootAttack(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketFlag::User_OnUserAttack_MagicAttack:
			pResult = (TryParsingMagicAttack(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketFlag::User_OnUserAttack_BodyAttack:
			pResult = (TryParsingBodyAttack(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketFlag::User_OnUserAttack_AreaDot:
			pResult = (TryParsingAreaDot(&attackInfo, nType, iPacket));
			break;
	}
	if (pResult)
	{
		m_pField->GetLifePool()->OnUserAttack(
			this,
			SkillInfo::GetInstance()->GetSkillByID(pResult->m_nSkillID),
			pResult
		);
	}
}

void User::OnLevelUp()
{
}

SecondaryStat * User::GetSecondaryStat()
{
	return this->m_pSecondaryStat;
}

BasicStat * User::GetBasicStat()
{
	return this->m_pBasicStat;
}

std::mutex & User::GetLock()
{
	return m_mtxUserlock;
}

void User::Update()
{
	m_pSecondaryStat->ResetByTime(this, GameDateTime::GetTime());
}

void User::ResetTemporaryStat(int tCur, int nReasonID)
{
	if (nReasonID == 0)
	{
		
	}
}

void User::OnStatChangeItemUseRequest(InPacket * iPacket, bool bByPet)
{
	int tTick = iPacket->Decode4();
	short nTI = iPacket->Decode2();
	int nItemID = iPacket->Decode4();

	auto pItem = m_pCharacterData->GetItem(2, nTI);
	auto pItemInfo = ItemInfo::GetInstance()->GetStateChangeItem(nItemID);
	if (pItem == nullptr || pItem->nItemID != nItemID || pItemInfo == nullptr)
	{
		SendCharacterStat(true, 0);
		return;
	}
	int nDecRet = 0;
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	bool bRemoveResult = QWUInventory::RawRemoveItem(
		this,
		2, // = Consume
		nTI,
		1,
		aChangeLog,
		nDecRet,
		nullptr);
	if(!bRemoveResult || nDecRet != 1)
	{
		SendCharacterStat(true, 0);
		return;
	}
	QWUInventory::SendInventoryOperation(this, false, aChangeLog);
	auto tsFlag = pItemInfo->Apply(this, GameDateTime::GetTime(), false);
	SendTemporaryStatReset(tsFlag);
	SendTemporaryStatSet(tsFlag, 0);
}

void User::OnStatChangeItemCancelRequest(InPacket * iPacket)
{
	int nItemID = -iPacket->Decode4();
	auto pItemInfo = ItemInfo::GetInstance()->GetStateChangeItem(nItemID);
	if (pItemInfo) 
	{
		auto tsFlag = pItemInfo->Apply(this, 0, false, true);
		SendTemporaryStatReset(tsFlag);
	}
}

void User::OnMigrateOut()
{
	LeaveField();
	m_pSocket->GetSocket().close();
}

void User::SetTransferStatus(TransferStatus e)
{
	m_nTransferStatus = e;
}

User::TransferStatus User::GetTransferStatus() const
{
	return m_nTransferStatus;
}

User * User::FindUser(int nUserID)
{
	return WvsGame::GetInstance<WvsGame>()->FindUser(nUserID);
}

void User::SendDropPickUpResultPacket(bool bPickedUp, bool bIsMoney, int nItemID, int nCount, bool bOnExcelRequest)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnMessage);
	oPacket.Encode1((char)Message::eDropPickUpMessage);
	if (bPickedUp)
	{
		oPacket.Encode1(bIsMoney == true ? 1 : 0);
		if (bIsMoney)
		{
			oPacket.Encode1(0);
			oPacket.Encode8(nCount);
			oPacket.Encode2(0);
		}
		else
		{
			oPacket.Encode4(nItemID);
			oPacket.Encode4(nCount);
		}
	}
	else
	{
		oPacket.Encode1((char)0xFF);
		oPacket.Encode4(0);
		oPacket.Encode4(0);
	}
	SendPacket(&oPacket);
}

void User::SendDropPickUpFailPacket(bool bOnExcelRequest)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnMessage);
	oPacket.Encode1((char)Message::eDropPickUpMessage);
	oPacket.Encode1((char)0xFE);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

Script * User::GetScript()
{
	return m_pScript;
}

void User::SetScript(Script * pScript)
{
	m_pScript = pScript;
}

void User::OnSelectNpc(InPacket * iPacket)
{
	auto pNpc = m_pField->GetLifePool()->GetNpc(iPacket->Decode4());
	if (pNpc != nullptr && GetScript() == nullptr)
	{
		auto pScript = ScriptMan::GetInstance()->GetScript(
			"./DataSrv/Script/Npc/" + std::to_string(pNpc->GetTemplateID()) + ".lua", 
			pNpc->GetTemplateID()
		);
		if (pScript == nullptr) 
		{
			SendChatMessage(4, "Npc : " + std::to_string(pNpc->GetTemplateID()) + " has no script.");
			return;
		}
		pScript->SetUser(this);
		SetScript(pScript);
		
		pScript->Run();
		//std::thread* t = new std::thread(&Script::Run, pScript);
		//pScript->SetThread(t);
		//t->detach();
	}
}

void User::OnScriptMessageAnswer(InPacket * iPacket)
{
	if (GetScript() != nullptr)
		m_pScript->OnPacket(iPacket);
}

void User::OnQuestRequest(InPacket * iPacket)
{
	char nAction = iPacket->Decode1();
	int nQuestID = iPacket->Decode4(), tTick, nItemID, nNpcID;
	NpcTemplate* pNpcTemplate = nullptr;
	Npc* pNpc = nullptr;
	if (nAction != 0 && nAction != 3)
	{
		nNpcID = iPacket->Decode4();
		pNpcTemplate = NpcTemplate::GetNpcTemplate(nNpcID);
		
		//if (pNpcTemplate == nullptr) // Invalid NPC Template
		//	return;

		/*if (!QuestMan::GetInstance()->IsAutoStartQuest(nQuestID))
		{
			pNpc = m_pField->GetLifePool()->GetNpc(nNpcID);
			if (!pNpc) // the npc not existed in this field
				return;
			auto rangeX = pNpc->GetPosX() - this->GetPosX();
			auto rangeY = pNpc->GetPosY() - this->GetPosY();

			if (abs(rangeX) > 1920 || abs(rangeY) > 1080)
			{
				WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "玩家%s疑似使用不當方式進行任務。任務 ID = %d, Npc ID = %d\n",
					this->m_pCharacterData->strName.c_str(),
					nQuestID,
					nNpcID);
				return;
			}
		}*/
	}

	WvsLogger::LogFormat("OnQuestRequest npc id = %d, quest action = %d\n", nNpcID, (int)nAction);
	switch (nAction)
	{
	case 0:
		tTick = iPacket->Decode4();
		nItemID = iPacket->Decode4();
		OnLostQuestItem(iPacket, nQuestID);
		break;
	case 1:
		OnAcceptQuest(iPacket, nQuestID, nNpcID, pNpc);
		break;
	case 2:
		OnCompleteQuest(iPacket, nQuestID, nNpcID, pNpc, QuestMan::GetInstance()->IsAutoCompleteQuest(nQuestID));
		break;
	case 3:
		OnResignQuest(iPacket, nQuestID);
		break;
	case 4:
		break;
	case 5:
		break;
	}
}

void User::OnAcceptQuest(InPacket * iPacket, int nQuestID, int dwTemplateID, Npc * pNpc)
{
	if (!QuestMan::GetInstance()->CheckStartDemand(nQuestID, this))
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[OnAcceptQuest]無法通過任務需求檢測，玩家名稱: %s, 任務ID: %d\n",
			m_pCharacterData->strName.c_str(),
			nQuestID);
		SendQuestResult(7, 0, 0);
		return;
	}
	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "任務檢測成功。\n");
	TryQuestStartAct(nQuestID, pNpc);
}

void User::OnCompleteQuest(InPacket * iPacket, int nQuestID, int dwTemplateID, Npc * pNpc, bool bIsAutoComplete)
{
	if (!QuestMan::GetInstance()->CheckCompleteDemand(nQuestID, this))
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[OnCompleteQuest]無法通過任務需求檢測，玩家名稱: %s, 任務ID: %d\n",
			m_pCharacterData->strName.c_str(),
			nQuestID);
		SendQuestResult(7, 0, 0);
		return;
	}
	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "任務檢測成功。\n");
	TryQuestCompleteAct(nQuestID, pNpc);
}

void User::OnResignQuest(InPacket * iPacket, int nQuestID)
{
	QWUQuestRecord::Remove(this, nQuestID, QWUQuestRecord::GetState(this, nQuestID) == 2);
}

void User::OnLostQuestItem(InPacket * iPacket, int nQuestID)
{
}

void User::TryQuestStartAct(int nQuestID, Npc * pNpc)
{
	auto pStartAct = QuestMan::GetInstance()->GetStartAct(nQuestID);
	if (!pStartAct)
		return;
	TryExchange(pStartAct->aActItem);
	QWUQuestRecord::Set(this, nQuestID, pStartAct->sInfo);
}

void User::TryQuestCompleteAct(int nQuestID, Npc * pNpc)
{
	auto pCompleteAct = QuestMan::GetInstance()->GetCompleteAct(nQuestID);
	if (!pCompleteAct)
		return;
	TryExchange(pCompleteAct->aActItem);
	QWUQuestRecord::SetComplete(this, nQuestID);
}

void User::TryExchange(const std::vector<ActItem*>& aActItem)
{
	std::vector<std::pair<int, int>> randItem;
	int nRandWeight = 0, nSelectedItemID = 0;
	for (auto& pItem : aActItem)
	{
		if (pItem->nProp > 0 && AllowToGetQuestItem(pItem)) 
		{
			randItem.push_back({ pItem->nProp, pItem->nItemID });
			nRandWeight += pItem->nProp;
		}
	}
	if (randItem.size() > 0) 
	{
		int randPos = Rand32::GetInstance()->Random() % nRandWeight;
		for(auto& rndItem : randItem)
			if ((randPos -= rndItem.first) <= 0)
			{
				nSelectedItemID = rndItem.second;
				break;
			}
	}
	for (auto& pItem : aActItem)
	{
		if (!AllowToGetQuestItem(pItem))
			continue;
		int nItemID = pItem->nItemID;
		if (pItem->nProp != 0 && nItemID != nSelectedItemID)
			continue;
		int nCount = pItem->nCount;
		if (nCount < 0)
			QWUInventory::RawRemoveItemByID(this, nItemID, nCount);
		else
			QWUInventory::RawAddItemByID(this, nItemID, nCount);
	}
}

bool User::AllowToGetQuestItem(const ActItem * pActionItem)
{
	if (pActionItem->nGender != 2 && pActionItem->nGender >= 0 && pActionItem->nGender != this->m_pBasicStat->nGender)
		return false;
	auto lmdCheckJob = [&](int encoded, int job) 
	{
		if ((encoded & 0x1) != 0) 
		{
			if ((0) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x2) != 0) 
		{
			if ((100) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x4) != 0) 
		{
			if ((200) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x8) != 0) 
		{
			if ((300) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x10) != 0) 
		{
			if ((400) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x20) != 0) 
		{
			if ((500) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x400) != 0) 
		{
			if ((1000) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x800) != 0) 
		{
			if ((1100) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x1000) != 0)
		{
			if ((1200) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x2000) != 0) 
		{
			if ((1300) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x4000) != 0) 
		{
			if ((1400) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x8000) != 0)
		{
			if ((1500) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x20000) != 0) 
		{
			if ((2001) / 100 == job / 100)
				return true;
			if ((2200) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x100000) != 0) 
		{
			if ((2000) / 100 == job / 100)
				return true;
			if ((2001) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x200000) != 0) 
		{
			if ((2100) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x400000) != 0) 
		{
			if ((2001) / 100 == job / 100)
				return true;
			if ((2200) / 100 == job / 100)
				return true;
		}

		if ((encoded & 0x40000000) != 0)
		{
			if ((3000) / 100 == job / 100)
				return true;
			if ((3200) / 100 == job / 100)
				return true;
			if ((3300) / 100 == job / 100)
				return true;
			if ((3500) / 100 == job / 100)
				return true;
		}
		return false;
	};
	auto lmdCheckJobEx = [&](int encoded, int job) 
	{
		if ((encoded & 0x1) != 0) 
		{
			if (((200) / 100) % 10 == (job / 100) % 10)
				return true;
		}
		if ((encoded & 0x2) != 0) 
		{
			if (((300) / 100) % 10 == (job / 100) % 10)
				return true;
		}
		if ((encoded & 0x4) != 0) 
		{
			if (((400) / 100) % 10 == (job / 100) % 10)
				return true;
		}
		if ((encoded & 0x8) != 0) 
		{
			if (((500) / 100) % 10 == (job / 100) % 10)
				return true;
		}
		return false;
	};
	if (pActionItem->nJob > 0) 
	{
		if (!lmdCheckJob(pActionItem->nJob, m_pBasicStat->nJob)
			&& !lmdCheckJobEx(pActionItem->nJobEx, m_pBasicStat->nJob))
			return false;
	}
	return true;
}

void User::SendQuestResult(int nResult, int nQuestID, int dwTemplateID)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketFlag::UserLocal_OnMessage);
	oPacket.Encode1(nResult);
	oPacket.Encode4(nQuestID);
	oPacket.Encode4(dwTemplateID);
	oPacket.Encode4(0);
	oPacket.Encode1(0);
	SendPacket(&oPacket);
}

void User::SendChatMessage(int nType, const std::string & sMsg)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketFlag::UserLocal_OnChatMsg);
	oPacket.Encode2((short)nType);
	oPacket.EncodeStr(sMsg);
	SendPacket(&oPacket);
}

void User::SendNoticeMessage(int nType, const std::string & sMsg)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketFlag::UserLocal_OnNoticeMsg);
	oPacket.EncodeStr(sMsg);
	oPacket.Encode1((unsigned char)nType);
	SendPacket(&oPacket);
}

void User::SendQuestRecordMessage(int nKey, int nState, const std::string & sStringRecord)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketFlag::UserLocal_OnMessage);
	oPacket.Encode1((char)Message::eQuestRecordMessage);
	oPacket.Encode4(nKey);
	oPacket.Encode1(nState);
	switch (nState)
	{
		case 0:
			oPacket.Encode1(0);
			break;
		case 1:
			oPacket.EncodeStr(sStringRecord);
			break;
		case 2:
			oPacket.Encode8(GameDateTime::GetTime());
			break;
	}
	SendPacket(&oPacket);
}

void User::LeaveField()
{
	m_pField->OnLeave(this);
}

void User::OnMigrateIn()
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnEventNameTag);
	for (int i = 0; i < 5; ++i)
	{
		oPacket.EncodeStr("");
		oPacket.Encode1(-1);
	}
	SendPacket(&oPacket);
	SendCharacterStat(true, 0);

	m_pField = (FieldMan::GetInstance()->GetField(m_pCharacterData->nFieldID));
	m_pField->OnEnter(this);
	auto bindT = std::bind(&User::Update, this);
	auto pUpdateTimer = AsyncScheduler::CreateTask(bindT, 2000, true);
	m_pUpdateTimer = pUpdateTimer;
	pUpdateTimer->Start();
	SetTransferStatus(TransferStatus::eOnTransferNone);
}
