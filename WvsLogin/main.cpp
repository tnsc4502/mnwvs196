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

#include "Constants\ConfigLoader.hpp"

void ConnectionAcceptorThread(short nPort)
{
	WvsLogin *loginServer = WvsBase::GetInstance<WvsLogin>();
	loginServer->CreateAcceptor(nPort);
	loginServer->BeginAccept<LoginSocket>();
}

int main(int argc, char** argv)
{
	WvsLogin *loginServer = WvsBase::GetInstance<WvsLogin>();

	if (argc > 1)
		ConfigLoader::GetInstance()->LoadConfig(argv[1]);
	else
	{
		std::cout << "Please run this program with command line, and given the config file path." << std::endl;
		return -1;
	}

	loginServer->Init();
	loginServer->InitializeCenter();

	std::thread initLoginServerThread(ConnectionAcceptorThread, ConfigLoader::GetInstance()->IntValue("port"));

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

