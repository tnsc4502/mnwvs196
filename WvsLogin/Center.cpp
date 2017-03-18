#include "Center.h"
#include <functional>
#include <thread>

#include "Net\InPacket.h"
#include "Net\OutPacket.h"

#include "Net\PacketFlags\LoginPacketFlags.hpp"
#include "Net\PacketFlags\CenterPacketFlags.hpp"

#include "Constants\ServerConstants.hpp"

Center::Center(asio::io_service& serverService)
	: SocketBase(serverService, true),
	  mResolver(serverService)
{
}

Center::~Center()
{
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

	//Encode center handshake packet.
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::RegisterCenterRequest);
	oPacket.Encode1(ServerConstants::ServerType::SVR_LOGIN);
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
		auto result = iPacket->Decode1();
		if (!result)
		{
			printf("[Warning]The Center Server Didn't Accept This Socket. Program Will Terminated.\n");
			exit(0);
		}
		printf("Center Server Authenciated Ok. The Connection Between Local Server Has Builded.\n");
	}
}

void Center::OnClosed()
{

}
