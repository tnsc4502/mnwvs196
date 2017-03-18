#include "ServerBase.h"
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

#include "Net\PacketFlags\LoginPacketFlags.hpp"
#include "Net\PacketFlags\CenterPacketFlags.hpp"

#include "Constants\ServerConstants.hpp"

ServerBase::ServerBase(asio::io_service& serverService)
	: SocketBase(serverService, true)
{
}

ServerBase::~ServerBase()
{
}

void ServerBase::OnClosed()
{

}

void ServerBase::OnPacket(InPacket *iPacket)
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

void ServerBase::OnRegisterCenterRequest(InPacket *iPacket)
{
	auto serverType = iPacket->Decode1();
	SetServerType(serverType);
	printf("Accept New %s\n", (serverType == ServerConstants::SVR_LOGIN ? "WvsLogin" : "WvsGame"));

	OutPacket oPacket;
	oPacket.Encode2(CenterPacketFlag::RegisterCenterAck);
	oPacket.Encode1(1); //Success;
	SendPacket(&oPacket);
}