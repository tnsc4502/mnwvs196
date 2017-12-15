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
#include "FieldMan.h"
#include "ItemInfo.h"
#include "SkillInfo.h"

#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotEquip.h"


void ConnectionAcceptorThread(short nPort)
{
	WvsGame *gameServer = WvsBase::GetInstance<WvsGame>();
	gameServer->CreateAcceptor(nPort);
	gameServer->BeginAccept<ClientSocket>();
}

#include "LifePool.h"
#include "..\Database\GA_Character.hpp"
#include "InventoryManipulator.h"


int main(int argc, char **argv)
{
	/*Calculator<int> c;
	
	for (int i = 0; i < 1000; ++i)
		std::cout << c.GetAnswer(c.Replace("x+10", 'x', std::to_string(i))) << std::endl;;*/
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
	std::cout << "TEst value = " << pItem ->liCashItemSN << " " << (pCharTest->GetItem(1, 1) == nullptr) << std::endl;
	pCharTest->Save(false);
	
	std::cout << "Ready to deallocate the test character data\n";
	system("Pause");
	delete pCharTest;
	system("Pause"); */
	ItemInfo::GetInstance()->Initialize();
	SkillInfo::GetInstance()->IterateSkillInfo();
	auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"];
	/*for (auto& eachMap : mapWz)
	{
		for (auto& map : eachMap)
		{
			if (map.Name() != "743040300") {
				//std::cout << map.Name() << std::endl;
				FieldMan::GetInstance()->GetField(atoi(map.Name().c_str()));
			}
		}
		//std::cout << eachMap << std::endl;
	}*/
	FieldMan::GetInstance()->GetField(261010102);
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

	WvsGameConstants::CenterServerList[0].nServerPort = ConfigLoader::GetInstance()->IntValue("Center0_Port");

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