#include "ClientSocket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Net\PacketFlags\GamePacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"

#include "WvsShop.h"
#include "User.h"

#include "..\WvsLib\Logger\WvsLogger.h"

ClientSocket::ClientSocket(asio::io_service& serverService)
	: SocketBase(serverService)
{
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::OnClosed()
{
}

void ClientSocket::OnPacket(InPacket *iPacket)
{
	iPacket->Print();
	//printf("\n");
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case LoginRecvPacketFlag::Client_ClientMigrateIn:
		OnMigrateIn(iPacket);
		break;
	default:
		if (pUser)
		{
			iPacket->RestorePacket();
			pUser->OnPacket(iPacket);
		}
	}
}

void ClientSocket::OnMigrateIn(InPacket *iPacket)
{
	//printf("OnMigrateIn\n");
	iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	auto pCenter = WvsBase::GetInstance<WvsShop>()->GetCenter();
	OutPacket oPacket;
	oPacket.Encode2(GameSendPacketFlag::RequestMigrateIn);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(nCharacterID);
	pCenter->SendPacket(&oPacket);
	//printf("OnMigrateOut\n");
}

void ClientSocket::SetUser(User *_pUser)
{
	pUser = _pUser;
}

User *ClientSocket::GetUser()
{
	return pUser;
}