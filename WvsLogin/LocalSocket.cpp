#include "LocalSocket.h"
#include "WvsWorld.h"
#include "Net\OutPacket.h"
#include "Net\InPacket.h"
#include "Net\crypto.hpp"
#include <functional>
#include <iostream>

LocalSocket::LocalSocket()
	: mSocket(WvsWorld::GetInstance()->GetIOService()),
	  aRecvIV((unsigned char*)stMemoryPoolMan->AllocateArray(16)),
	  aSendIV((unsigned char*)stMemoryPoolMan->AllocateArray(16))
{
}

LocalSocket::~LocalSocket()
{
	stMemoryPoolMan->DestructArray(aRecvIV);
	stMemoryPoolMan->DestructArray(aSendIV);
}

void LocalSocket::Init()
{
	OutPacket *oPacket = new OutPacket();
	oPacket->Encode2(15);
	oPacket->Encode2(196);
	oPacket->EncodeStr("3");
	oPacket->Encode4(*(int*)aRecvIV);
	oPacket->Encode4(*(int*)aSendIV);
	oPacket->Encode1(6);
	oPacket->Encode1(0);

	SendPacket(*oPacket);
	OnWaitingPacket();
}

asio::ip::tcp::socket& LocalSocket::GetSocket()
{
	return mSocket;
}

void LocalSocket::SendPacket(OutPacket &iPacket)
{
	asio::async_write(mSocket,
		asio::buffer(iPacket.GetPacket(), iPacket.GetPacketSize() + 4),
		std::bind(&LocalSocket::OnSendPacketFinished,
			shared_from_this(), std::placeholders::_1, std::placeholders::_2, iPacket.GetPacket()));
}

void LocalSocket::OnSendPacketFinished(const std::error_code &ec, std::size_t bytes_transferred, unsigned char *buffer)
{
	stMemoryPoolMan->DestructArray(buffer);
}

void LocalSocket::OnWaitingPacket()
{
	aRecivedPacket.reset((unsigned char*)stMemoryPoolMan->AllocateArray(4));	
	asio::async_read(mSocket,
		asio::buffer(aRecivedPacket.get(), 4),
		std::bind(&LocalSocket::OnReceive,
			shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void LocalSocket::OnReceive(const std::error_code &ec, std::size_t bytes_transferred)
{
	if (!ec)
	{
		unsigned short nPacketLen = crypto::get_packet_length(aRecivedPacket.get());

		if (nPacketLen < 2)
		{
			//disconnect();
			return;
		}

		aRecivedPacket.reset((unsigned char*)stMemoryPoolMan->AllocateArray(nPacketLen));

		asio::async_read(mSocket,
			asio::buffer(aRecivedPacket.get(), nPacketLen),
			std::bind(&LocalSocket::ProcessPacket,
				shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}
	else
	{
		//disconnect();
	}
}

void LocalSocket::ProcessPacket(const std::error_code &ec, std::size_t bytes_transferred)
{
	if (!ec)
	{
		unsigned short nBytes = static_cast<unsigned short>(bytes_transferred);

		crypto::decrypt(aRecivedPacket.get(), aRecvIV, nBytes);
		InPacket iPacket(aRecivedPacket.get(), nBytes);
		iPacket.Print();
		/*std::cout << "On Received Packet:";
		for (int i = 0; i < bytes_amount; ++i)
			printf("0x%0X ", (int)aRecivedPacket.get()[i]);
			//std::cout << (int)aRecivedPacket.get()[i] << " " ;
		std::cout << std::endl;*/
		// handle the packet
		//player_->handle_packet(bytes_amount);

		// start an async read operation to receive the header of the next packet
		OnWaitingPacket();
	}
	else
	{
	}
}