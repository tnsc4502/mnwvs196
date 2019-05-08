// WvsGame.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <thread>
#include "LocalServer.h"
#include "WvsCenter.h"
#include "WvsWorld.h"

#include "..\WvsLib\Net\InPacket.h"

#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\Database\WvsUnified.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterList.hpp"
#include "..\Database\GW_ItemSlotEquip.h"

#include "..\WvsGame\User.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsCenter *centerServer = WvsBase::GetInstance<WvsCenter>();
	centerServer->CreateAcceptor(nPort);
	centerServer->BeginAccept<LocalServer>();
}

int main(int argc, char **argv)
{
	ConfigLoader* pConfigLoader = nullptr;
	if (argc > 1)
		pConfigLoader = ConfigLoader::Get(argv[1]);
	else
	{
		std::cout << "Please run this program with command line, and given the config file path." << std::endl;
		return -1;
	}
	WvsBase::GetInstance<WvsCenter>()->Init();

	WvsWorld::GetInstance()->SetConfigLoader(pConfigLoader);
	WvsWorld::GetInstance()->InitializeWorld();

	// start the connection acceptor thread

	std::thread thread1(ConnectionAcceptorThread, (pConfigLoader->IntValue("port")));

	// start the i/o work

	asio::io_service &io = WvsBase::GetInstance<WvsCenter>()->GetIOService();
	asio::io_service::work work(io);

	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
}

