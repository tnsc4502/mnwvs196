#include "Center.h"
#include <functional>
#include "Net\InPacket.h"
#include "Net\OutPacket.h"
#include <thread>

Center::Center(asio::io_service& serverService)
	: SocketBase(serverService),
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
	OutPacket oPacket;
	oPacket.Encode2(0xFFFF);
	oPacket.Encode1(1);
	SendPacket(&oPacket); 
	OnWaitingPacket();
}

void Center::OnPacket(InPacket *iPacket)
{
	printf("[Center::OnPacket]");
	iPacket->Print();
}

void Center::OnClosed()
{

}
