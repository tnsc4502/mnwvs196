#include "LoginSocket.h"
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

#include "Net\PacketFlags\LoginPacketFlags.hpp"
#include "Net\PacketFlags\ClientPacketFlags.hpp"

LoginSocket::LoginSocket(asio::io_service& serverService)
	: SocketBase(serverService)
{
}


LoginSocket::~LoginSocket()
{
}

void LoginSocket::OnPacket(InPacket *iPacket)
{
	printf("[LoginSocket::OnPacket]");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();

	switch (nType)
	{
	case ClientPacketFlag::ClientRequestStart:
		OnClientRequestStart();
		break;
	case ClientPacketFlag::LoginBackgroundRequest:
		OnLoginBackgroundRequest();
	}
}

void LoginSocket::OnClosed()
{

}

void LoginSocket::OnClientRequestStart()
{
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::ClientStartResponse);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

void LoginSocket::OnLoginBackgroundRequest()
{
	static std::string backgrounds[] = { "MapLogin", "MapLogin1", "MapLogin2" };
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::LoginBackgroundResponse);
	oPacket.EncodeStr(backgrounds[rand() % (sizeof(backgrounds) / sizeof(backgrounds[0]))]);
	oPacket.Encode4(0);
	oPacket.Encode1(1);
	SendPacket(&oPacket);
}