#pragma once
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsLib\Net\WorldInfo.h"
#include "..\WvsLib\Common\ServerConstants.hpp"
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Logger\WvsLogger.h"

class Center :
	public SocketBase
{
private:
	int nCenterIndex;
	WorldInfo m_WorldInfo;

	void OnConnected();

public:
	Center(asio::io_service& serverService);
	~Center();

	void SetCenterIndex(int idx);

	void OnPacket(InPacket *iPacket);
	void OnClosed();
	void OnConnectFailed();

	const WorldInfo& GetWorldInfo()
	{
		return m_WorldInfo;
	}

	void OnUpdateChannelInfo(InPacket *iPacket);
	void OnUpdateWorldInfo(InPacket *iPacket);
	void OnCharacterListResponse(InPacket *iPacket);
	void OnGameServerInfoResponse(InPacket *iPacket);
	static void OnNotifyCenterDisconnected(SocketBase *pSocket);
};

