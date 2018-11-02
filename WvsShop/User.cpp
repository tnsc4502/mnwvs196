#include "User.h"
#include "ClientSocket.h"
#include "WvsShop.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_Avatar.hpp"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"

#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\ShopPacketFlags.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"

User::User(ClientSocket *_pSocket, InPacket *iPacket)
	: m_pSocket(_pSocket),
	m_pCharacterData(AllocObj(GA_Character))
{
	_pSocket->SetUser(this);
	m_pCharacterData->DecodeCharacterData(iPacket, true);
	if (!iPacket->Decode1())
		m_nChannelID = 0;
	else
		m_nChannelID = iPacket->Decode4();
	auto bindT = std::bind(&User::Update, this);
	m_pUpdateTimer = AsyncScheduler::CreateTask(bindT, 2000, true);
	m_pUpdateTimer->Start();
	//m_pSecondaryStat->DecodeInternal(this, iPacket);
}

User::~User()
{
	WvsLogger::LogRaw("Migrate Out, Encoding Character data to center.\n");
	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketFlag::RequestMigrateOut);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	m_pCharacterData->EncodeCharacterData(&oPacket, true);
	oPacket.Encode1(1); //bGameEnd, Dont decode and save the secondarystat info.
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);

	auto bindT = std::bind(&User::Update, this);
	m_pUpdateTimer->Abort();


	FreeObj( m_pCharacterData );
}


int User::GetUserID() const
{
	return m_pCharacterData->nCharacterID;
}

void User::SendPacket(OutPacket *oPacket)
{
	m_pSocket->SendPacket(oPacket);
}

void User::OnPacket(InPacket *iPacket)
{
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case UserRecvPacketFlag::User_OnUserTransferFieldRequest:
			OnMigrateOutCashShop();
			break;
	}
}

void User::Update()
{

}

void User::OnMigrateOutCashShop()
{
	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketFlag::RequestTransferToGame);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(m_nCharacterID);
	oPacket.Encode1(m_nChannelID);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}
