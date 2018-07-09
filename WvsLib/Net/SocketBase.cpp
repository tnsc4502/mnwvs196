#include "SocketBase.h"
#include "..\Constants\ServerConstants.hpp"
#include "OutPacket.h"
#include "InPacket.h"
#include <functional>
#include <iostream>

#include "..\Crypto\WvsCrypto.hpp"
#include "..\Logger\WvsLogger.h"

std::mutex SocketBase::stSocketRecordMtx;
std::set<unsigned int> SocketBase::stSocketIDRecord;


SocketBase::SocketBase(asio::io_service& serverService, bool isLocalServer)
	: mSocket(serverService),
	mResolver(serverService),
	bIsLocalServer(isLocalServer),
	nSocketID(SocketBase::DesignateSocketID()),
	aRecvIV((unsigned char*)MSMemoryPoolMan::GetInstance()->AllocateArray(16)),
	aSendIV((unsigned char*)MSMemoryPoolMan::GetInstance()->AllocateArray(16))
{
}

SocketBase::~SocketBase()
{
	MSMemoryPoolMan::GetInstance()->DestructArray(aRecvIV);
	MSMemoryPoolMan::GetInstance()->DestructArray(aSendIV);
}

void SocketBase::SetSocketDisconnectedCallBack(const std::function<void(SocketBase *)>& fObject)
{
	m_fSocketDisconnectedCallBack = fObject;
}

void SocketBase::SetServerType(unsigned char type)
{
	nServerType = type;
}

unsigned char SocketBase::GetServerType()
{
	return nServerType;
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
	m_eSocketStatus = SocketStatus::eClosed;
	ReleaseSocketID(GetSocketID());
	m_fSocketDisconnectedCallBack(this);
	mSocket.close();
	OnClosed();
}

void SocketBase::Connect(const std::string & strAddr, short nPort)
{
	m_eSocketStatus = SocketStatus::eConnecting;
	asio::ip::tcp::resolver::query centerSrvQuery(strAddr, std::to_string(nPort));

	mResolver.async_resolve(centerSrvQuery,
		std::bind(&SocketBase::OnResolve, std::dynamic_pointer_cast<SocketBase>(shared_from_this()),
			std::placeholders::_1,
			std::placeholders::_2));
}

void SocketBase::OnResolve(const std::error_code & err, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
	{
		asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
		GetSocket().async_connect(endpoint,
			std::bind(&SocketBase::OnConnectResult, std::static_pointer_cast<SocketBase>(shared_from_this()),
				std::placeholders::_1, ++endpoint_iterator));
	}
	else
	{
		OnConnectFailed();
		return;
	}
}

void SocketBase::OnConnectResult(const std::error_code & err, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (err)
	{
		m_eSocketStatus = SocketStatus::eClosed;
		OnConnectFailed();
	}
	else
	{
		m_eSocketStatus = SocketStatus::eConnected;
		OnConnected();
	}
}

void SocketBase::OnConnected()
{
}

void SocketBase::OnConnectFailed()
{
}

unsigned int SocketBase::DesignateSocketID()
{
	static bool bRandSeedSet = false;
	if (!bRandSeedSet)
	{
		srand((unsigned int)time(nullptr));
		bRandSeedSet = true;
	}
	const unsigned int RESERVED_INDEX = 100;
	const unsigned int MAX_RAND_RANGE = RESERVED_INDEX + (INT_MAX - 1 - RESERVED_INDEX) * 2;
	long long int liRnd = 0;
	unsigned int nRndSocketID = 0;
	std::lock_guard<std::mutex> lock(stSocketRecordMtx);
	do
	{
		liRnd = (long long int)(rand() % 0x7FFF) * (long long int)(rand() % 0x7FFF) * (long long int)(rand() % 0x7FFF) * (long long int)(rand() % 0x7FFF);
		nRndSocketID = RESERVED_INDEX + liRnd % ((long long int)MAX_RAND_RANGE);
	} 
	while ((stSocketIDRecord.find(nRndSocketID) != stSocketIDRecord.end()));
	stSocketIDRecord.insert(nRndSocketID);
	return nRndSocketID;
}

void SocketBase::ReleaseSocketID(unsigned int nSocketID)
{
	std::lock_guard<std::mutex> lock(stSocketRecordMtx);
	stSocketIDRecord.erase(nSocketID);
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

SocketBase::SocketStatus SocketBase::GetSocketStatus() const
{
	return m_eSocketStatus;
}

bool SocketBase::CheckSocketStatus(SocketStatus e) const
{
	return (m_eSocketStatus & e) == e;
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
		WvsCrypto::create_packet_header(buffPtr - 4, aSendIV, oPacket->GetPacketSize());
		if (!bIsLocalServer)
			WvsCrypto::encrypt(buffPtr, aSendIV, oPacket->GetPacketSize());
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
	//aRecivedPacket.reset((unsigned char*)MSMemoryPoolMan::GetInstance()->AllocateArray(4));
	asio::async_read(mSocket,
		asio::buffer(buffer, 4),
		std::bind(&SocketBase::OnReceive,
			shared_from_this(), std::placeholders::_1, std::placeholders::_2, buffer));
}

void SocketBase::OnReceive(const std::error_code &ec, std::size_t bytes_transferred, unsigned char* buffer)
{
	if (!ec)
	{
		unsigned short nPacketLen = WvsCrypto::get_packet_length(buffer);
		//printf("[SocketBase::OnReceive] Packet Size = %d\n", nPacketLen);
		if (nPacketLen < 2)
		{
			OnDisconnect();
			return;
		}

		delete[] buffer;
		buffer = new unsigned char[nPacketLen];
		//aRecivedPacket.reset((unsigned char*)MSMemoryPoolMan::GetInstance()->AllocateArray(nPacketLen));

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
			WvsCrypto::decrypt(buffer, aRecvIV, nBytes);
		InPacket iPacket(buffer, nBytes);
		try {
			this->OnPacket(&iPacket);
		}
		catch (std::exception& ex) {
			iPacket.RestorePacket();
			WvsLogger::LogFormat("解析封包時發生錯誤，OPCode : %d, 異常訊息 : %s\n", (int)iPacket.Decode2(), ex.what());
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