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

#include "..\Common\Utility\Task\AsnycScheduler.h"
#include "Constants\ConfigLoader.hpp"
//#include "..\WvsLib\Logger\WvsLogger.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsLogin *loginServer = WvsBase::GetInstance<WvsLogin>();
	loginServer->CreateAcceptor(nPort);
	loginServer->BeginAccept<LoginSocket>();
}

void Count()
{
	for (int i = 0; i < 100000; ++i)
		WvsLogger::LogFormat("%d\n", i);
}

int main(int argc, char** argv)
{
	/*auto func = std::bind(Count);
	auto pTest = AsnycScheduler::CreateTask(func, 100, false);
	pTest->Start();*/
	WvsLogin *loginServer = WvsBase::GetInstance<WvsLogin>();

	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "Please run this program with command line, and given the config file path.\n");
	WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Error Code : %d\n", 10000);
	if (argc > 1)
		ConfigLoader::GetInstance()->LoadConfig(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
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

