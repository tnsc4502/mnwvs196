#include "SocketBase.h"
#include "Constants\ServerConstants.hpp"
#include "Net\OutPacket.h"
#include "Net\InPacket.h"
#include "Net\crypto.hpp"
#include <functional>
#include <iostream>

//0~99«O¯d
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

	if(!bIsLocalServer)
		SendPacket(oPacket.get(), true);

	OnWaitingPacket();
}

void SocketBase::OnDisconnect()
{
	printf("[SocketBase::OnDisconnect]\n");
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
	if (!mSocket.is_open())
	{
		OnDisconnect();
		return;
	}
	auto buffPtr = oPacket->GetPacket();
	if (!handShakePacket)
	{
		crypto::create_packet_header(buffPtr -4, aSendIV, oPacket->GetPacketSize());
		if(!bIsLocalServer)
			crypto::encrypt(buffPtr, aSendIV, oPacket->GetPacketSize());
		asio::async_write(mSocket,
			asio::buffer(buffPtr - 4, oPacket->GetPacketSize() + 4),
			std::bind(&SocketBase::OnSendPacketFinished,
				shared_from_this(), std::placeholders::_1, std::placeholders::_2, buffPtr - 4));
	}
	else
	{
		asio::async_write(mSocket,
			asio::buffer(buffPtr, oPacket->GetPacketSize()),
			std::bind(&SocketBase::OnSendPacketFinished,
				shared_from_this(), std::placeholders::_1, std::placeholders::_2, buffPtr - 4));
	}
}

void SocketBase::OnSendPacketFinished(const std::error_code &ec, std::size_t bytes_transferred, unsigned char *buffer)
{
	stMemoryPoolMan->DestructArray(buffer);
}

void SocketBase::OnWaitingPacket()
{
	aRecivedPacket.reset((unsigned char*)stMemoryPoolMan->AllocateArray(4));
	asio::async_read(mSocket,
		asio::buffer(aRecivedPacket.get(), 4),
		std::bind(&SocketBase::OnReceive,
			shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void SocketBase::OnReceive(const std::error_code &ec, std::size_t bytes_transferred)
{
	if (!ec)
	{
		unsigned short nPacketLen = crypto::get_packet_length(aRecivedPacket.get());

		if (nPacketLen < 2)
		{
			OnDisconnect();
			return;
		}

		aRecivedPacket.reset((unsigned char*)stMemoryPoolMan->AllocateArray(nPacketLen));

		asio::async_read(mSocket,
			asio::buffer(aRecivedPacket.get(), nPacketLen),
			std::bind(&SocketBase::ProcessPacket,
				shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}
	else
		OnDisconnect();
}

void SocketBase::ProcessPacket(const std::error_code &ec, std::size_t bytes_transferred)
{
	if (!ec)
	{
		unsigned short nBytes = static_cast<unsigned short>(bytes_transferred);

		if(!bIsLocalServer)
			crypto::decrypt(aRecivedPacket.get(), aRecvIV, nBytes);
		InPacket iPacket(aRecivedPacket.get(), nBytes);
		this->OnPacket(&iPacket);
		OnWaitingPacket();
	}
}

unsigned int SocketBase::GetSocketID() const
{
	return nSocketID;
}