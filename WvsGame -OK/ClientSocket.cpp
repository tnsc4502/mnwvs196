#include "ClientSocket.h"
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

#include "Net\PacketFlags\GamePacketFlags.hpp"
#include "Net\PacketFlags\ClientPacketFlags.hpp"
#include "WvsGame.h"

#include "User.h"

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
	printf("[WvsGame][ClientSocket::OnPacket]«Ê¥]±µ¦¬¡G");
	iPacket->Print();
	//printf("\n");
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case ClientPacketFlag::ClientMigrateIn:
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
	printf("OnMigrateIn\n");
	auto pCenter = WvsBase::GetInstance<WvsGame>()->GetCenter();
	iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	OutPacket oPacket;
	oPacket.Encode2(GamePacketFlag::RequestMigrateIn);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(nCharacterID);
	pCenter->SendPacket(&oPacket);
	printf("OnMigrateOut\n");
}

void ClientSocket::SetUser(User *_pUser)
{
	pUser = _pUser;
}

User *ClientSocket::GetUser()
{
	return pUser;
}