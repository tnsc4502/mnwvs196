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

#include "Constants\ConfigLoader.hpp"
#include "FieldMan.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsGame *gameServer = WvsBase::GetInstance<WvsGame>();
	gameServer->CreateAcceptor(nPort);
	gameServer->BeginAccept<ClientSocket>();
}

#include "LifePool.h"

int main(int argc, char **argv)
{
	/*FieldMan::GetInstance()->GetField(100000000);
	FieldMan::GetInstance()->GetField(100000000).GetLifePool()->PrintNpc();
	system("pause");*/
	WvsBase::GetInstance<WvsGame>()->Init();
	if (argc > 1)
		ConfigLoader::GetInstance()->LoadConfig(argv[1]);
	else
	{
		std::cout << "Please run this program with command line, and given the config file path." << std::endl;
		return -1;
	}

	WvsBase::GetInstance<WvsGame>()->SetExternalIP(ConfigLoader::GetInstance()->StrValue("externalIP"));
	WvsBase::GetInstance<WvsGame>()->SetExternalPort(ConfigLoader::GetInstance()->IntValue("port"));
	// start the connection acceptor thread

	std::thread thread1(ConnectionAcceptorThread, ConfigLoader::GetInstance()->IntValue("port"));

	//WvsGameConstants::nGamePort = (argc > 1 ? atoi(argv[1]) : 7575);
	//WvsGameConstants::strGameDesc = "Channel : " + std::to_string((argc > 1 ? atoi(argv[1]) : 7575) - 7575);

	WvsGameConstants::CenterServerList[0].nServerPort = ConfigLoader::GetInstance()->IntValue("Center0_Port");

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