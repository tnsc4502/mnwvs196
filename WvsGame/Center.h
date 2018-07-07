#pragma once
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsLib\Constants\WvsGameConstants.hpp"

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

	CenterInfo m_WorldInfo;

	void OnResolve(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);
	void OnConnect(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);

public:
	Center(asio::io_service& serverService);
	~Center();

	void OnClosed();

	const CenterInfo& GetWorldInfo()
	{
		return m_WorldInfo;
	}

	bool IsConnected() const { return m_WorldInfo.bIsConnected; }
	bool IsConnectionFailed() const { return m_WorldInfo.bConnectionFailed; }

	void SetCenterIndex(int idx);

	void OnConnectToCenter(const std::string& strAddr, short nPort);
	void OnPacket(InPacket *iPacket);
	void OnCenterMigrateInResult(InPacket *iPacket);
	void OnTransferChannelResult(InPacket *iPacket);
	void OnMigrateCashShopResult(InPacket *iPacket);

	static void OnNotifyCenterDisconnected(SocketBase *pSocket);
};

