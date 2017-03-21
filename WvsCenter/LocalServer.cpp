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
	case LoginPacketFlag::RequestCharacterList:
		OnRequestCharacterList(iPacket);
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
	if (serverType == ServerConstants::SVR_LOGIN)
	{
		auto pWorld = WvsBase::GetInstance<WvsCenter>();
		oPacket.Encode1(pWorld->GetWorldInfo().nWorldID);
		oPacket.Encode1(pWorld->GetWorldInfo().nEventType);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strWorldDesc);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strEventDesc);
		printf("[LocalServer::OnRegisterCenterRequest]Encoding World Information.\n");
	}
	SendPacket(&oPacket);
}

void LocalServer::OnRequestCharacterList(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();
	//CharacterDBAccessor::GetCharacterList(nAccountID, WvsBase::GetInstance<WvsCenter>()->GetWorldInfo().nWorldID);

	OutPacket oPacket;
	oPacket.Encode2(CenterPacketFlag::CharacterListResponse);
	oPacket.Encode4(nLoginSocketID);
	oPacket.Encode4(0); //size of chars

	oPacket.Encode1(0); //size of chars
	
	SendPacket(&oPacket);
}