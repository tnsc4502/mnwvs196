// WvsGame.cpp : Defines the entry point for the console application.
//

#include "Net\asio.hpp"
#include <iostream>
#include <thread>
#include "Net\InPacket.h"
#include "Net\OutPacket.h"
#include "WvsLogin.h"
#include "LoginSocket.h"
#include "WvsLoginConstants.hpp"

void ConnectionAcceptorThread()
{
	WvsLogin *loginServer = WvsBase::GetInstance<WvsLogin>();
	loginServer->CreateAcceptor(8484);
	loginServer->BeginAccept<LoginSocket>();
}

int main()
{
	WvsLogin *loginServer = WvsBase::GetInstance<WvsLogin>();

	loginServer->Init();
	loginServer->InitializeCenter();
	std::thread initLoginServerThread(ConnectionAcceptorThread);

	// start the i/o work

	asio::io_service &io = WvsBase::GetInstance<WvsLogin>()->GetIOService();

	asio::io_service::work work(io);

	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
	while (1);
}

