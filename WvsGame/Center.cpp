#include "Center.h"
#include <functional>
#include <thread>

#include "Net\InPacket.h"
#include "Net\OutPacket.h"

#include "Net\PacketFlags\LoginPacketFlags.hpp"
#include "Net\PacketFlags\CenterPacketFlags.hpp"
#include "Net\PacketFlags\GamePacketFlags.hpp"

#include "Constants\ServerConstants.hpp"

#include "WvsGame.h"
#include "User.h"
#include "FieldMan.h"

Center::Center(asio::io_service& serverService)
	: SocketBase(serverService, true),
	  mResolver(serverService)
{
}

Center::~Center()
{
}

void Center::SetCenterIndex(int idx)
{
	nCenterIndex = idx;
}

void Center::OnConnectToCenter(const std::string& strAddr, short nPort)
{
	asio::ip::tcp::resolver::query centerSrvQuery(strAddr, std::to_string(nPort)); 
	
	mResolver.async_resolve(centerSrvQuery,
		std::bind(&Center::OnResolve, std::dynamic_pointer_cast<Center>(shared_from_this()),
			std::placeholders::_1,
			std::placeholders::_2));
}

void Center::OnResolve(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
	{
		asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
		GetSocket().async_connect(endpoint,
			std::bind(&Center::OnConnect, std::dynamic_pointer_cast<Center>(shared_from_this()),
				std::placeholders::_1, ++endpoint_iterator));
	}
	else
	{
	}
}

void Center::OnConnect(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (err)
	{
		printf("[Center::OnConnect]Connect Failed, Center Server No Response.\n");
		OnDisconnect();
		return;
	}
	printf("[Center::OnConnect]Connect To Center Server Successed!\n");
	mWorldInfo.bIsConnected = true;
	//Encode center handshake packet.
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::RegisterCenterRequest);
	oPacket.Encode1(ServerConstants::ServerType::SVR_GAME);
	auto ip = WvsBase::GetInstance<WvsGame>()->GetExternalIP();
	for (int i = 0; i < 4; ++i)
		oPacket.Encode1((unsigned char)ip[i]);
	oPacket.Encode2(WvsBase::GetInstance<WvsGame>()->GetExternalPort());

	InPacket iPacket(oPacket.GetPacket(), oPacket.GetPacketSize());
	printf("[WvsGame::Center::]Ready to send handshake info.");
	iPacket.Print();
	printf("\n");
	SendPacket(&oPacket);

	OnWaitingPacket();
}

void Center::OnPacket(InPacket *iPacket)
{
	printf("[Center::OnPacket]");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case CenterPacketFlag::RegisterCenterAck:
	{
		auto result = iPacket->Decode1();
		if (!result)
		{
			printf("[Warning]The Center Server Didn't Accept This Socket. Program Will Terminated.\n");
			exit(0);
		}
		printf("Center Server Authenciated Ok. The Connection Between Local Server Has Builded.\n");
		break;
	}
	case CenterPacketFlag::CenterMigrateInResult:
		OnCenterMigrateInResult(iPacket);
		break;
	}
}

void Center::OnClosed()
{

}

void Center::OnCenterMigrateInResult(InPacket *iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsGame>()->GetSocketList()[nClientSocketID];
	OutPacket oPacket;
	oPacket.Encode2(GamePacketFlag::ClientMigrateIn);
	oPacket.Encode4(0); //Channel ID
	oPacket.Encode1(0);
	oPacket.Encode4(0);
	oPacket.Encode1(1); //bCharacterData
	oPacket.Encode4(0);
	oPacket.Encode4(800);
	oPacket.Encode4(600);
	oPacket.Encode1(1); //bCharacterData
	oPacket.Encode2(0);

	oPacket.Encode4(rand() % 10000);
	oPacket.Encode4(rand() % 10000);
	oPacket.Encode4(rand() % 10000);

	oPacket.EncodeBuffer(iPacket->GetPacket() + 6, iPacket->GetPacketSize() - 6);
	for (int i = 0; i < 5; ++i)
		oPacket.Encode4(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.EncodeTime(-1); //TIME

	oPacket.EncodeHexString("64 00 00 00 00 00 00 01 A6 00 00 00 03 00 00 00 83 7D 26 5A 02 00 00 24 66 00 00 00 00 00 03 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 40 E0 FD 3B 37 4F 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 82 16 FB 52 01 00 00 24 0C 00 00 00 00 00 00 00 00 00 00 00 C8 00 00 00 F7 24 11 76 00 00 00 24 0C 00 00 00 01 00 00 24 02 00 00 24 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 96 00 00 00 00");
	for (int i = 0; i < 1000; ++i)
		oPacket.Encode8(i);
	pSocket->SendPacket(&oPacket);

	std::shared_ptr<User> newUser{ new User((ClientSocket*)pSocket, iPacket) };
	WvsBase::GetInstance<WvsGame>()->OnUserConnected(newUser);
}