#pragma once
#include "Net\asio.hpp"
#include "Memory\MemoryPoolMan.hpp"

class OutPacket;

//Client ºÝ¤§ Session
class LocalSocket : public std::enable_shared_from_this<LocalSocket>
{
private:
	struct UniquePtrDeeleter
	{
		void operator()(unsigned char* ptr)
		{
			stMemoryPoolMan->DestructArray(ptr);
		}
	};

	asio::ip::tcp::socket mSocket;
	unsigned char* aRecvIV, *aSendIV;
	std::unique_ptr<unsigned char, UniquePtrDeeleter> aRecivedPacket;

	void OnSendPacketFinished(const std::error_code &ec, std::size_t bytes_transferred, unsigned char *buffer);

public:
	LocalSocket();
	~LocalSocket();

	void Init();
	void SendPacket(OutPacket &iPacket);
	void OnWaitingPacket();
	void OnReceive(const std::error_code &ec, std::size_t bytes_transferred);
	void ProcessPacket(const std::error_code &ec, std::size_t bytes_transferred);

	asio::ip::tcp::socket& GetSocket();
};

