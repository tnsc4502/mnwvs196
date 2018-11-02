// WvsShop.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <thread>
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "WvsShop.h"
#include "ClientSocket.h"
#include "..\WvsLib\Common\WvsLoginConstants.hpp"

#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
//#include "..\WvsLib\Logger\WvsLogger.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsShop *pShopServer = WvsBase::GetInstance<WvsShop>();
	pShopServer->CreateAcceptor(nPort);
	pShopServer->BeginAccept<ClientSocket>();
}

int main(int argc, char** argv)
{
	WvsShop *pShopServer = WvsBase::GetInstance<WvsShop>();

	if (argc > 1)
		ConfigLoader::GetInstance()->LoadConfig(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
		return -1;
	}
	pShopServer->SetExternalIP(ConfigLoader::GetInstance()->StrValue("ExternalIP"));
	pShopServer->SetExternalPort(ConfigLoader::GetInstance()->IntValue("Port"));

	pShopServer->Init();

	std::thread pShopSrvThread(ConnectionAcceptorThread, ConfigLoader::GetInstance()->IntValue("Port"));
	pShopServer->InitializeCenter();

	// start the i/o work

	asio::io_service &io = WvsBase::GetInstance<WvsShop>()->GetIOService();

	asio::io_service::work work(io);

	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
}

