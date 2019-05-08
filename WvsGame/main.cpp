// WvsGame.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <thread>

#include "QuestMan.h"
#include <functional>

#include "ClientSocket.h"
#include "WvsGame.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "FieldMan.h"
#include "TimerThread.h"
#include "NpcTemplate.h"
#include "ReactorTemplate.h"

#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_MobReward.h"
#include "..\Database\GW_Shop.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsGame *gameServer = WvsBase::GetInstance<WvsGame>();
	gameServer->CreateAcceptor(nPort);
	gameServer->BeginAccept<ClientSocket>();
}

int main(int argc, char **argv)
{
	TimerThread::RegisterTimerPool(50, 1000);
	ReactorTemplate::Load();
	FieldMan::GetInstance()->LoadFieldSet();
	QuestMan::GetInstance()->LoadAct();
	QuestMan::GetInstance()->LoadDemand();
	ItemInfo::GetInstance()->Initialize();
	GW_MobReward::GetInstance()->Load();
	GW_Shop::GetInstance()->Load();
	NpcTemplate::GetInstance()->Load();
	try {
		SkillInfo::GetInstance()->IterateSkillInfo();
	}
	catch (...) {}

	ConfigLoader* pCfgLoader = nullptr;
	WvsBase::GetInstance<WvsGame>()->Init();
	if (argc > 1)
		pCfgLoader = ConfigLoader::Get(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
		return -1;
	}

	WvsBase::GetInstance<WvsGame>()->SetExternalIP(pCfgLoader->StrValue("ExternalIP"));
	WvsBase::GetInstance<WvsGame>()->SetExternalPort(pCfgLoader->IntValue("Port"));
	// start the connection acceptor thread

	std::thread thread1(ConnectionAcceptorThread, pCfgLoader->IntValue("Port"));

	WvsBase::GetInstance<WvsGame>()->SetConfigLoader(pCfgLoader);
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