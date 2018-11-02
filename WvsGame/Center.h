#pragma once
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"

class Center :
	public SocketBase
{
private:
	int nCenterIndex;

	void OnConnected();

public:
	Center(asio::io_service& serverService);
	~Center();

	void OnClosed();
	void OnConnectFailed();
	void SetCenterIndex(int idx);

	void OnPacket(InPacket *iPacket);
	void OnCenterMigrateInResult(InPacket *iPacket);
	void OnTransferChannelResult(InPacket *iPacket);

	static void OnNotifyCenterDisconnected(SocketBase *pSocket);
};

