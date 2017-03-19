// WvsGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Net\asio.hpp"
#include <iostream>
#include <thread>
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

#include "ClientSocket.h"
#include "WvsGame.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsGame *gameServer = WvsBase::GetInstance<WvsGame>();
	gameServer->CreateAcceptor(nPort);
	gameServer->BeginAccept<ClientSocket>();
}

int main(int argc, char **argv)
{
	WvsBase::GetInstance<WvsGame>()->Init();

	// start the connection acceptor thread

	std::thread thread1(ConnectionAcceptorThread, (argc > 1 ? atoi(argv[1]) : 7575));

	WvsGameConstants::nGamePort = (argc > 1 ? atoi(argv[1]) : 7575);
	WvsGameConstants::strGameDesc = "Channel : " + std::to_string((argc > 1 ? atoi(argv[1]) : 7575) - 7575);

	WvsGameConstants::CenterServerList[0].nServerPort = (argc > 2 ? atoi(argv[2]) : 8484);

	WvsBase::GetInstance<WvsGame>()->InitializeCenter();
	// start the i/o work

	asio::io_service &io = WvsBase::GetInstance<WvsGame>()->GetIOService();
	asio::io_service::work work(io);

	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
}