#include "SocketBase.h"
#include "Constants\ServerConstants.hpp"
#include "Net\OutPacket.h"
#include "Net\InPacket.h"
#include "Net\crypto.hpp"
#include <functional>
#include <iostream>

//0~99保留
unsigned int SocketBase::SocketCount = 100;

SocketBase::SocketBase(asio::io_service& serverService, bool isLocalServer)
	: mSocket(serverService),
	bIsLocalServer(isLocalServer),
	nSocketID(SocketCount++),
	aRecvIV((unsigned char*)stMemoryPoolMan->AllocateArray(16)),
	aSendIV((unsigned char*)stMemoryPoolMan->AllocateArray(16))
{
	//????
	if (SocketCount >= 4294940000)
		SocketCount = 0;
}

SocketBase::~SocketBase()
{
	stMemoryPoolMan->DestructArray(aRecvIV);
	stMemoryPoolMan->DestructArray(aSendIV);
}

void SocketBase::Init()
{
	asio::ip::tcp::no_delay option(true);
	mSocket.set_option(option);

	std::unique_ptr<OutPacket> oPacket{ new OutPacket() };
	EncodeHandShakeInfo(oPacket.get());

	if (!bIsLocalServer)
		SendPacket(oPacket.get(), true);

	OnWaitingPacket();
}

void SocketBase::OnDisconnect()
{
	//printf("[SocketBase::OnDisconnect]\n");
	OnNotifySocketDisconnected(this);
	mSocket.close();
	OnClosed();
}

void SocketBase::EncodeHandShakeInfo(OutPacket *oPacket)
{
	oPacket->Encode2(0x0F);
	oPacket->Encode2(ServerConstants::kGameVersion);
	oPacket->EncodeStr(ServerConstants::strGameSubVersion);
	oPacket->Encode4(*(int*)aRecvIV);
	oPacket->Encode4(*(int*)aSendIV);
	oPacket->Encode1(ServerConstants::kGameLocale);
	oPacket->Encode1(0x00);
}

asio::ip::tcp::socket& SocketBase::GetSocket()
{
	return mSocket;
}

void SocketBase::SendPacket(OutPacket *oPacket, bool handShakePacket)
{
	oPacket->IncRefCount();
	if (!mSocket.is_open())
	{
		OnDisconnect();
		return;
	}
	auto buffPtr = oPacket->GetPacket();
	if (!handShakePacket)
	{
		crypto::create_packet_header(buffPtr - 4, aSendIV, oPacket->GetPacketSize());
		if (!bIsLocalServer)
			crypto::encrypt(buffPtr, aSendIV, oPacket->GetPacketSize());
		asio::async_write(mSocket,
			asio::buffer(buffPtr - 4, oPacket->GetPacketSize() + 4),
			std::bind(&SocketBase::OnSendPacketFinished,
				shared_from_this(), std::placeholders::_1, std::placeholders::_2, buffPtr - (4 + 8), oPacket->GetSharedPacket()));
	}
	else
	{
		asio::async_write(mSocket,
			asio::buffer(buffPtr, oPacket->GetPacketSize()),
			std::bind(&SocketBase::OnSendPacketFinished,
				shared_from_this(), std::placeholders::_1, std::placeholders::_2, buffPtr - (4 + 8), oPacket->GetSharedPacket()));
	}
}

void SocketBase::OnSendPacketFinished(const std::error_code &ec, std::size_t bytes_transferred, unsigned char *buffer, void *pPacket)
{
	((OutPacket::SharedPacket*)pPacket)->DecRefCount();
}

void SocketBase::OnWaitingPacket()
{
	auto buffer = new unsigned char[4];
	//aRecivedPacket.reset((unsigned char*)stMemoryPoolMan->AllocateArray(4));
	asio::async_read(mSocket,
		asio::buffer(buffer, 4),
		std::bind(&SocketBase::OnReceive,
			shared_from_this(), std::placeholders::_1, std::placeholders::_2, buffer));
}

void SocketBase::OnReceive(const std::error_code &ec, std::size_t bytes_transferred, unsigned char* buffer)
{
	if (!ec)
	{
		unsigned short nPacketLen = crypto::get_packet_length(buffer);
		//printf("[SocketBase::OnReceive] Packet Size = %d\n", nPacketLen);
		if (nPacketLen < 2)
		{
			OnDisconnect();
			return;
		}

		delete[] buffer;
		buffer = new unsigned char[nPacketLen];
		//aRecivedPacket.reset((unsigned char*)stMemoryPoolMan->AllocateArray(nPacketLen));

		asio::async_read(mSocket,
			asio::buffer(buffer, nPacketLen),
			std::bind(&SocketBase::ProcessPacket,
				shared_from_this(), std::placeholders::_1, std::placeholders::_2, buffer));
	}
	else
		OnDisconnect();
}

void SocketBase::ProcessPacket(const std::error_code &ec, std::size_t bytes_transferred, unsigned char* buffer)
{
	if (!ec)
	{
		unsigned short nBytes = static_cast<unsigned short>(bytes_transferred);

		if (!bIsLocalServer)
			crypto::decrypt(buffer, aRecvIV, nBytes);
		InPacket iPacket(buffer, nBytes);
		try {
			this->OnPacket(&iPacket);
		}
		catch (std::exception& ex) {
			iPacket.RestorePacket();
			std::cout << "解析封包時發生錯誤，OPCode : " << iPacket.Decode2() << " Err : " << ex.what() << std::endl;
			iPacket.Print();
		}
		delete[] buffer;
		OnWaitingPacket();
	}
}

unsigned int SocketBase::GetSocketID() const
{
	return nSocketID;
}