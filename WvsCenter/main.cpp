// WvsGame.cpp : Defines the entry point for the console application.
//

#include "Net\asio.hpp"
#include <iostream>
#include <thread>
#include "LocalServer.h"
#include "WvsCenter.h"

#include "Net\InPacket.h"

#include "Constants\ConfigLoader.hpp"
#include "..\Database\WvsUnified.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterList.hpp"
#include "..\Database\GW_ItemSlotEquip.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsCenter *centerServer = WvsBase::GetInstance<WvsCenter>();
	centerServer->CreateAcceptor(nPort);
	centerServer->BeginAccept<LocalServer>();
}

int main(int argc, char **argv)
{
	//WvsUnified unifiedDB;
	//unifiedDB.(1);


	/*GW_CharacterList list;
	list.Load(0, 0);

	GA_Character ga;
	ga.Load(4);*/
	//std::cout << "Size of EQP = " << ga.mItemSlot[1].size() << std::endl;
	//system("pause");

	auto pConfigLoader = ConfigLoader::GetInstance();
	if (argc > 1)
		pConfigLoader->LoadConfig(argv[1]);
	else
	{
		std::cout << "Please run this program with command line, and given the config file path." << std::endl;
		return -1;
	}
	WvsBase::GetInstance<WvsCenter>()->Init();

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

