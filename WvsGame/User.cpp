#include "User.h"
#include "ClientSocket.h"

#include "..\Database\GA_Character.hpp"

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
	oPacket.Encode4(50); //HP

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

void User::LeaveField()
{
	pField->OnLeave(this);
}