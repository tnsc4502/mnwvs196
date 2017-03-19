#include "LocalServer.h"
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

#include "Net\PacketFlags\LoginPacketFlags.hpp"
#include "Net\PacketFlags\CenterPacketFlags.hpp"

#include "Constants\ServerConstants.hpp"
#include "WvsCenter.h"

LocalServer::LocalServer(asio::io_service& serverService)
	: SocketBase(serverService, true)
{
}

LocalServer::~LocalServer()
{
}

void LocalServer::OnClosed()
{

}

void LocalServer::OnPacket(InPacket *iPacket)
{
	printf("[ServerBase::OnPacket]");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case LoginPacketFlag::RegisterCenterRequest:
		OnRegisterCenterRequest(iPacket);
		break;
	}
}

void LocalServer::OnRegisterCenterRequest(InPacket *iPacket)
{
	auto serverType = iPacket->Decode1();
	SetServerType(serverType);
	printf("Accept New %s\n", (serverType == ServerConstants::SVR_LOGIN ? "WvsLogin" : "WvsGame"));

	if (serverType == ServerConstants::SVR_GAME)
	{
		WvsBase::GetInstance<WvsCenter>()->RegisterChannel(shared_from_this());
		WvsBase::GetInstance<WvsCenter>()->NotifyWorldChanged();
	}

	OutPacket oPacket;
	oPacket.Encode2(CenterPacketFlag::RegisterCenterAck);
	oPacket.Encode1(1); //Success;
	SendPacket(&oPacket);
}
