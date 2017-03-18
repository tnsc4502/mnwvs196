// MNMS.cpp : Defines the entry point for the console application.
//

//#include "WzResMan.hpp"
//#include "wzfile.hpp"
//#include "wznode.hpp"
#include "asio.hpp"
#include <list>
#include <iostream>

#include "WzResMan.hpp"
#include "MemoryPoolMan.hpp"

#include "LocalSocket.h"
#include "WvsWorld.h"

#include "InPacket.h"
#include "OutPacket.h"

void ConnectionAcceptorThread()
{
	WvsWorld *world = WvsWorld::GetInstance();

	auto endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8484);
	asio::ip::tcp::acceptor acceptor(world->GetIOService(), endpoint);

	//while (world->is_accepting_connections())
	while (true)
	{
		std::shared_ptr<LocalSocket> session(new LocalSocket());

		std::error_code ec;
		acceptor.accept(session->GetSocket(), ec);

		if (!ec)
		{
			std::cout << "New Client Accepted" << std::endl;
			session->Init();
		}
	}
}

int main()
{
	unsigned char data[] = { 0x07, 0x00, 0xB1 , 0x4D , 0xC4 , 0xDD , 0xBD , 0xFC , 0x37 };

	InPacket iPacket(data, sizeof(data) / sizeof(char));

	std::cout << "On Read : " << iPacket.DecodeStr() << std::endl;

	OutPacket oPacket;
	oPacket.EncodeStr("±MÄÝ½ü7");

	auto buff = oPacket.GetPacket();
	for (int i = 0; i < oPacket.GetPacketSize(); ++i)
		printf("0x%02X ", buff[i]);

	WvsWorld::GetInstance()->Init();

	// start the connection acceptor thread

	std::thread thread1(ConnectionAcceptorThread);

	// start the i/o work

	asio::io_service &io = WvsWorld::GetInstance()->GetIOService();
	asio::io_service::work work(io);

	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
	/*auto testPtr = stMemoryPoolMan->AllocateArray(100);

	stMemoryPoolMan->DestructArray(testPtr);
	std::cout << "END\n";
	std::cout << "END\n";
	system("pause");
	std::cout << "END\n";
	auto lst = stWzResMan->GetWz(Wz::Map)["Map"];
	std::cout << lst.begin().begin().Name();
	for (int i = 0; i <10; ++i)
	{
		auto eInfo = lst["Map" + to_string(i + 1)];
		for (auto item : eInfo)
		{
			//std::cout << item.Name() << std::endl;
			for (auto info : item["info"])
			{

			}
		}
	}
	std::cout << "END\n";
	stWzResMan->GetWz(Wz::Map)["Map"].ReleaseData();
	system("pause");
	stWzResMan->GetWz(Wz::Character)["Weapon"];
	for (auto e : lst)
	{
		//std::cout << e.Name() << std::endl;
		auto eInfo = e["info"];
		for (auto info : eInfo)
		{
			//std::cout << info.Name() << std::endl;
		}
	}
	stWzResMan->GetWz(Wz::Character)["Weapon"].ReleaseData();
	std::cout << "END\n";
	//stWzResMan->GetWz(Wz::Character)["Map"].ReleaseData();
	system("pause");
	lst = stWzResMan->GetWz(Wz::Character)["Shield"];
	for (auto e : lst)
	{
		//std::cout << e.Name() << std::endl;
		auto eInfo = e["info"];
		for (auto info : eInfo)
		{
			//std::cout << info.Name() << std::endl;
		}
	}*/


	return 0;
}


/*	std::cout << "Start Loading Map\n";
	auto lst = stWzResMan->GetWz(Wz::Map)["Map"];
	std::cout << lst.begin().begin().Name();
	for (int i = 0; i <10; ++i)
	{
		auto eInfo = lst["Map" + to_string(i + 1)];
		for (auto item : eInfo)
		{
			//std::cout << item.Name() << std::endl;
			for (auto info : item["info"])
			{

			}
		}
	}
	std::cout << "Clearing Map\n";
	stWzResMan->GetWz(Wz::Map)["Map"].ReleaseData();
	std::cout << "Start Loadingn Weapon\n";
	stWzResMan->GetWz(Wz::Character)["Weapon"];
	for (auto e : lst)
	{
		//std::cout << e.Name() << std::endl;
		auto eInfo = e["info"];
		for (auto info : eInfo)
		{
			//std::cout << info.Name() << std::endl;
		}
	}
	stWzResMan->GetWz(Wz::Character)["Weapon"].ReleaseData();
	std::cout << "Clearing Weapon\nStart Loading Shield\n";
	lst = stWzResMan->GetWz(Wz::Character)["Shield"];
	for (auto e : lst)
	{
		//std::cout << e.Name() << std::endl;
		auto eInfo = e["info"];
		for (auto info : eInfo)
		{
			//std::cout << info.Name() << std::endl;
		}
	}

	std::cout << "Clearing ShieldRelease Memory\n" << endl;
	stWzResMan->ReleaseMemory();

	std::cout << "Start Loading Map\n";
	lst = stWzResMan->GetWz(Wz::Map)["Map"];
	std::cout << lst.begin().begin().Name();
	for (int i = 0; i <10; ++i)
	{
		auto eInfo = lst["Map" + to_string(i + 1)];
		for (auto item : eInfo)
		{
			//std::cout << item.Name() << std::endl;
			for (auto info : item["info"])
			{

			}
		}
	}
	std::cout << "Clearing Map\n";
	
	stWzResMan->GetWz(Wz::Map)["Map"].ReleaseData();

	std::cout << "Start Loadingn Weapon\n";
	stWzResMan->GetWz(Wz::Character)["Weapon"];
	for (auto e : lst)
	{
		//std::cout << e.Name() << std::endl;
		auto eInfo = e["info"];
		for (auto info : eInfo)
		{
			//std::cout << info.Name() << std::endl;
		}
	}
	stWzResMan->GetWz(Wz::Character)["Weapon"].ReleaseData();
	std::cout << "Clearing Weapon\nStart Loading Shield\n";
	lst = stWzResMan->GetWz(Wz::Character)["Shield"];
	for (auto e : lst)
	{
		//std::cout << e.Name() << std::endl;
		auto eInfo = e["info"];
		for (auto info : eInfo)
		{
			//std::cout << info.Name() << std::endl;
		}
	}

	std::cout << "Clearing Shield" << endl;*/
