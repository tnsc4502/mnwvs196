// WvsGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Net\asio.hpp"
#include <iostream>
#include <thread>
#include "Net\InPacket.h"
#include "Net\OutPacket.h"
#include "WvsWorld.h"
#include "LocalSocket.h"

void ConnectionAcceptorThread()
{
	WvsWorld *world = WvsWorld::GetInstance();

	auto endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8484);
	asio::ip::tcp::acceptor acceptor(world->GetIOService(), endpoint);

	//while (world->is_accepting_connections())
	while (true)
	{
		std::shared_ptr<SocketBase> session(new SocketBase());

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
	unsigned char data[] = { 0x08, 0x00, 0x63 , 0x6F , 0x75 , 0x6E , 0x74 , 0x3D , 0x34 , 0x30 };

	InPacket iPacket(data, sizeof(data) / sizeof(char));

	std::cout << "On Read : " << iPacket.DecodeStr() << std::endl;

	OutPacket oPacket;
	oPacket.EncodeStr("±MÄÝ½ü7");

	auto buff = oPacket.GetPacket();
	for (int i = 0; i < oPacket.GetPacketSize(); ++i)
		printf("0x%02X ", buff[i]);
	system("pause");
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

}

