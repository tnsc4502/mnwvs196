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

#include "AdminSkills.h"
#include "AngelicBusterSkills.h"
#include "AranSkills.h"
#include "BattleMageSkills.h"
#include "BeastTamerSkills.h"
#include "BeginnersSkills.h"
#include "BlasterSkills.h"
#include "BlazeWizardSkills.h"
#include "BowmanSkills.h"
#include "DawnWarriorSkills.h"
#include "DemonSlayerSkills.h"
#include "EvanSkills.h"
#include "HayatoSkills.h"
#include "KaiserSkills.h"
#include "KannaSkills.h"
#include "KinesisSkills.h"
#include "LegendarySkills.h"
#include "LuminousSkills.h"
#include "MagicSkills.h"
#include "ManagerSkills.h"
#include "MechanicSkills.h"
#include "MercedesSkills.h"
#include "MihileSkills.h"
#include "NamelessWardenSkills.h"
#include "NightWalkerSkills.h"
#include "NoblesseSkills.h"
#include "PhantomSkills.h"
#include "PinkBeanSkills.h"
#include "PirateSkills.h"
#include "ResistanceSkills.h"
#include "RogueSkills.h"
#include "ShadeSkills.h"
#include "ThunderBreakerSkills.h"
#include "WarriorSkills.h"
#include "WildHunterSkills.h"
#include "WindArcherSkills.h"
#include "XenonSkills.h"
#include "ZeroSkills.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsGame *gameServer = WvsBase::GetInstance<WvsGame>();
	gameServer->CreateAcceptor(nPort);
	gameServer->BeginAccept<ClientSocket>();
}

void CheckSkillInfoLoading(int *i)
{
	printf("%d::::::::::::::::%p::::::::::::::::::::::%d\n", *i, i, rand());
	/*int count = SkillInfo::GetInstance()->GetLoadingSkillCount();
	if (count == 0)
		stWzResMan->ReleaseMemory();*/
}

void SaveSkillTest()
{
	/*GA_Character pCharTest;
	pCharTest.Load(11);
	std::cout << "Skill Count : " << pCharTest.GetCharacterSkillRecord().size() << std::endl;
	pCharTest.GetSkill(100100)->nSLV = 15;
	pCharTest.Save();*/
}

void SaveItemTest()
{
	/*GA_Character* pCharTest;
	std::vector<InventoryManipulator::ChangeLog> aList;
	int nDecRemove = 0;
	std::cout << "Ready to allocate a new test character data\n";
	system("Pause");
	OutPacket oPacket;
	(pCharTest = new GA_Character())->Load(11);

	GW_ItemSlotEquip* pItem = ((GW_ItemSlotEquip*)pCharTest->GetItem(1, -11));
	std::cout << "TEst value = " << pItem->liCashItemSN << " " << (pCharTest->GetItem(1, 1) == nullptr) << std::endl;

	InventoryManipulator::RawRemoveItem(pCharTest, 3, 1, 2, aList, &nDecRemove, nullptr);
	//InventoryManipulator::RawAddItem(pCharTest, 1, pItemClone, aList, &nDecRemove);
	//pCharTest->Save(false);
	//InventoryManipulator::RawRemoveItem(pCharTest, 1, 1001, 1, aList, &nDecRemove, nullptr);
	pCharTest->EncodeCharacterData(&oPacket);
	delete pCharTest;
	InPacket iPacket(oPacket.GetPacket(), oPacket.GetPacketSize());

	(pCharTest = new GA_Character());
	pCharTest->DecodeCharacterData(&iPacket);
	pItem = ((GW_ItemSlotEquip*)pCharTest->GetItem(1, -11));
	std::cout << "TEst value = " << pItem->liCashItemSN << " " << (pCharTest->GetItem(1, 1) == nullptr) << std::endl;
	pCharTest->Save(false);

	std::cout << "Ready to deallocate the test character data\n";
	system("Pause");
	delete pCharTest;
	system("Pause");*/
}

void AsyncTimerTest(int i)
{
	//auto bindResult = std::bind(CheckSkillInfoLoading, i);
	//auto t = AsyncScheduler::CreateTask(bindResult, 500, true);
	//t->Start();
}

int main(int argc, char **argv)
{
	//WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
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

	WvsBase::GetInstance<WvsGame>()->Init();
	if (argc > 1)
		ConfigLoader::GetInstance()->LoadConfig(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
		return -1;
	}

	WvsBase::GetInstance<WvsGame>()->SetExternalIP(ConfigLoader::GetInstance()->StrValue("ExternalIP"));
	WvsBase::GetInstance<WvsGame>()->SetExternalPort(ConfigLoader::GetInstance()->IntValue("Port"));
	// start the connection acceptor thread

	std::thread thread1(ConnectionAcceptorThread, ConfigLoader::GetInstance()->IntValue("Port"));

	//WvsGameConstants::nGamePort = (argc > 1 ? atoi(argv[1]) : 7575);
	//WvsGameConstants::strGameDesc = "Channel : " + std::to_string((argc > 1 ? atoi(argv[1]) : 7575) - 7575);

	//WvsGameConstants::CenterServerList[0].nServerPort = ConfigLoader::GetInstance()->IntValue("Center0_Port");

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