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
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "..\Common\Utility\Task\AsnycScheduler.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_MobReward.h"

#include <functional>

void ConnectionAcceptorThread(short nPort)
{
	WvsGame *gameServer = WvsBase::GetInstance<WvsGame>();
	gameServer->CreateAcceptor(nPort);
	gameServer->BeginAccept<ClientSocket>();
}

void CheckSkillInfoLoading(int i)
{
	printf("%d:%d\n", i, rand());
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
	auto bindResult = std::bind(CheckSkillInfoLoading, i);
	auto t = AsnycScheduler::CreateTask(bindResult, 500, true);
	t->Start();
}

int main(int argc, char **argv)
{
	//printf("Test %d", WvsGameConstants::GetJobLevel(434));
	/*GA_Character chr;
	chr.Load(11);
	OutPacket oPacket;
	chr.EncodeCharacterData(&oPacket);

	InPacket iPacket(oPacket.GetPacket(), oPacket.GetPacketSize());
	chr.DecodeCharacterData(&iPacket);*/
	//for (int i = 0; i < 100000; ++i)
	//	AsyncTimerTest(i);
	GW_MobReward::GetInstance()->Load();
	auto pReward = GW_MobReward::GetInstance()->GetMobReward(100100);
	auto& ref = pReward->GetRewardList();
	//for (const auto& pInfo : ref)
	//	printf("R %d\n", pInfo->nItemID);
	ItemInfo::GetInstance()->Initialize();
	SkillInfo::GetInstance()->IterateSkillInfo();

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