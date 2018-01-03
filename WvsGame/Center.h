#pragma once
#include "Net\SocketBase.h"
#include "WvsGameConstants.hpp"

class Center :
	public SocketBase
{
private:
	struct CenterInfo
	{
		int nCenterID;
		bool bIsConnected = false, bConnectionFailed = false;
	};

	int nCenterIndex;

	asio::ip::tcp::resolver mResolver;

	CenterInfo mWorldInfo;

	void OnResolve(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);
	void OnConnect(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);

public:
	Center(asio::io_service& serverService);
	~Center();

	void OnClosed();

	const CenterInfo& GetWorldInfo()
	{
		return mWorldInfo;
	}

	bool IsConnected() const { return mWorldInfo.bIsConnected; }
	bool IsConnectionFailed() const { return mWorldInfo.bConnectionFailed; }

	void SetCenterIndex(int idx);

	void OnConnectToCenter(const std::string& strAddr, short nPort);
	void OnPacket(InPacket *iPacket);
	void OnCenterMigrateInResult(InPacket *iPacket);

	static void OnNotifyCenterDisconnected(SocketBase *pSocket)
	{
		printf("[WvsLogin][Center]與Center Server中斷連線。\n");
		((Center*)pSocket)->mWorldInfo.bIsConnected = false;
		((Center*)pSocket)->mWorldInfo.bConnectionFailed = true;
	}
};

