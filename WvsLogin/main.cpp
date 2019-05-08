// WvsGame.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <thread>
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "WvsLogin.h"
#include "LoginSocket.h"
#include "..\WvsLib\Common\WvsLoginConstants.hpp"

#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
//#include "..\WvsLib\Logger\WvsLogger.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsLogin *pLoginServer = WvsBase::GetInstance<WvsLogin>();
	pLoginServer->CreateAcceptor(nPort);
	pLoginServer->BeginAccept<LoginSocket>();
}

int main(int argc, char** argv)
{
	WvsLogin *pLoginServer = WvsBase::GetInstance<WvsLogin>();
	ConfigLoader* pConfigLoader = nullptr;

	if (argc > 1)
		pConfigLoader = ConfigLoader::Get(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
		return -1;
	}

	pLoginServer->SetConfigLoader(pConfigLoader);
	pLoginServer->Init();
	pLoginServer->InitializeCenter();

	std::thread initLoginServerThread(ConnectionAcceptorThread, pConfigLoader->IntValue("port"));

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

