#include "ClientSocket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Net\PacketFlags\GamePacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "WvsGame.h"
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
			if (nType == 0x96) {
				WvsLogger::LogRaw("[WvsGame][ClientSocket::OnPacket]«Ê¥]±µ¦¬¡G");
				iPacket->Print();
			}
			pUser->OnPacket(iPacket);
		}
	}
}

void ClientSocket::OnMigrateIn(InPacket *iPacket)
{
	//printf("OnMigrateIn\n");
	auto pCenter = WvsBase::GetInstance<WvsGame>()->GetCenter();
	iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
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