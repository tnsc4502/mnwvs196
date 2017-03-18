// WvsGame.cpp : Defines the entry point for the console application.
//

#include "Net\asio.hpp"
#include <iostream>
#include <thread>
#include "LocalServer.h"
#include "ServerBase.h"

#include "Wz\WzResMan.hpp"
#include "Net\InPacket.h"


void ConnectionAcceptorThread()
{
	LocalServer *localServer = WvsBase::GetInstance<LocalServer>();
	localServer->CreateAcceptor(8384);
	localServer->BeginAccept<ServerBase>();
}

int main()
{
	WvsBase::GetInstance<LocalServer>()->Init();

	// start the connection acceptor thread

	std::thread thread1(ConnectionAcceptorThread);

	// start the i/o work

	asio::io_service &io = WvsBase::GetInstance<LocalServer>()->GetIOService();
	asio::io_service::work work(io);

	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
}

