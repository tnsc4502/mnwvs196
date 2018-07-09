// WvsGame.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <thread>
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "WvsLogin.h"
#include "LoginSocket.h"
#include "..\WvsLib\Constants\WvsLoginConstants.hpp"

#include "..\WvsLib\Task\AsnycScheduler.h"
#include "..\WvsLib\Constants\ConfigLoader.hpp"
//#include "..\WvsLib\Logger\WvsLogger.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsLogin *pLoginServer = WvsBase::GetInstance<WvsLogin>();
	pLoginServer->CreateAcceptor(nPort);
	pLoginServer->BeginAccept<LoginSocket>();
}

void Count()
{
	for (int i = 0; i < 100000; ++i)
		WvsLogger::LogFormat("%d\n", i);
}

int main(int argc, char** argv)
{
	/*測試SOCKETID配給的效能與隨機分布
	for (int i = 0; i < 10000000; ++i)
	{
		unsigned int rnd = SocketBase::DesignateSocketID();
		SocketBase::ReleaseSocketID(rnd);
	}
	while (1)
	{
		unsigned int rnd = SocketBase::DesignateSocketID();
		std::system("pause");
	}*/
		
	/*auto func = std::bind(Count);
	auto pTest = AsnycScheduler::CreateTask(func, 100, false);
	pTest->Start();*/
	WvsLogin *pLoginServer = WvsBase::GetInstance<WvsLogin>();

	if (argc > 1)
		ConfigLoader::GetInstance()->LoadConfig(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
		return -1;
	}

	pLoginServer->Init();
	pLoginServer->InitializeCenter();

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

