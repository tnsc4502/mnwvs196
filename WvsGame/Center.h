#pragma once
#include "Net\SocketBase.h"
#include "WvsGameConstants.h"

class Center :
	public SocketBase
{
private:
	struct CenterInfo
	{
		int nCenterID;
		bool bIsConnected;
	};

	int nCenterIndex;

	asio::ip::tcp::resolver mResolver;

	CenterInfo mWorldInfo;

	void OnResolve(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);
	void OnConnect(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);

public:
	Center(asio::io_service& serverService);
	~Center();

	void SetCenterIndex(int idx);

	void OnConnectToCenter(const std::string& strAddr, short nPort);
	void OnPacket(InPacket *iPacket);
	void OnClosed();

	const CenterInfo& GetWorldInfo()
	{
		return mWorldInfo;
	}
};

