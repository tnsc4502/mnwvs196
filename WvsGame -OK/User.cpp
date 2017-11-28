#include "User.h"
#include "ClientSocket.h"

#include "..\Database\GA_Character.hpp"

#include "..\Common\Net\OutPacket.h"
#include "..\Common\Net\InPacket.h"

#include "..\Common\Net\PacketFlags\ClientPacketFlags.hpp"
#include "..\Common\Net\PacketFlags\UserPacketFlags.h"

#include "FieldMan.h"
#include "Field.h"

User::User(ClientSocket *_pSocket, InPacket *iPacket)
	: pSocket(_pSocket),
	  pCharacterData(new GA_Character())
{
	pCharacterData->DecodeCharacterData(iPacket);
	_pSocket->SetUser(this);
	pField = (FieldMan::GetInstance()->GetField(pCharacterData->nFieldID));
	pField->OnEnter(this);
}

User::~User()
{
	pField->OnLeave(this);
}

int User::GetUserID() const
{
	return pCharacterData->nCharacterID;
}

void User::SendPacket(OutPacket *oPacket)
{
	pSocket->SendPacket(oPacket);
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
	default:
		if (pField)
		{
			iPacket->RestorePacket();
			pField->OnPacket(iPacket);
		}
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