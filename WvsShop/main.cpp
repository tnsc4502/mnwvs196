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
#include "..\WvsGame\ItemInfo.h"
#include "ShopInfo.h"
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
	ItemInfo::GetInstance()->Initialize();
	ShopInfo::GetInstance()->Init();

	ConfigLoader* pCfgLoader = nullptr;
	if (argc > 1)
		pCfgLoader = ConfigLoader::Get(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
		return -1;
	}
	pShopServer->SetExternalIP(pCfgLoader->StrValue("ExternalIP"));
	pShopServer->SetExternalPort(pCfgLoader->IntValue("Port"));

	pShopServer->Init();

	std::thread pShopSrvThread(ConnectionAcceptorThread, pCfgLoader->IntValue("Port"));
	pShopServer->SetConfigLoader(pCfgLoader);
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

