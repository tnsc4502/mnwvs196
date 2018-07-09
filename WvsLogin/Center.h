#pragma once
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsLib\Net\WorldInfo.h"
#include "..\WvsLib\Constants\ServerConstants.hpp"
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Logger\WvsLogger.h"

class Center :
	public SocketBase
{
private:

	int nCenterIndex;
	bool bIsConnected = false, bConnectionFailed = false;
	asio::ip::tcp::resolver mResolver;

	WorldInfo m_WorldInfo;

	void OnResolve(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);
	void OnConnect(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);

public:
	Center(asio::io_service& serverService);
	~Center();

	void SetCenterIndex(int idx);

	void OnConnectToCenter(const std::string& strAddr, short nPort);
	void OnPacket(InPacket *iPacket);
	void OnClosed();
	void OnConnectFailed();

	const WorldInfo& GetWorldInfo()
	{
		return m_WorldInfo;
	}

	bool IsConnectionFailed() const
	{
		return bConnectionFailed;
	}

	bool IsConnected() const
	{
		return bIsConnected;
	}

	void OnUpdateChannelInfo(InPacket *iPacket);
	void OnUpdateWorldInfo(InPacket *iPacket);
	void OnCharacterListResponse(InPacket *iPacket);
	void OnGameServerInfoResponse(InPacket *iPacket);

	static void OnNotifyCenterDisconnected(SocketBase *pSocket)
	{
		WvsLogger::LogRaw("[WvsLogin][Center]與Center Server中斷連線。\n");
		((Center*)pSocket)->bIsConnected = false;
		((Center*)pSocket)->bConnectionFailed = true;
	}
};

