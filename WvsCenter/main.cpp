// WvsGame.cpp : Defines the entry point for the console application.
//

#include "Net\asio.hpp"
#include <iostream>
#include <thread>
#include "LocalServer.h"
#include "WvsCenter.h"

#include "Wz\WzResMan.hpp"
#include "Net\InPacket.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsCenter *centerServer = WvsBase::GetInstance<WvsCenter>();
	centerServer->CreateAcceptor(nPort);
	centerServer->BeginAccept<LocalServer>();
}

int main(int argc, char **argv)
{
	WvsBase::GetInstance<WvsCenter>()->Init();

	// start the connection acceptor thread

	std::thread thread1(ConnectionAcceptorThread, (argc > 1 ? atoi(argv[1]) : 8383));

	// start the i/o work

	asio::io_service &io = WvsBase::GetInstance<WvsCenter>()->GetIOService();
	asio::io_service::work work(io);

	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
}

