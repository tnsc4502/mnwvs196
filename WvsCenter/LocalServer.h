#pragma once
#include "Net\SocketBase.h"

//Server ºÝ¤§ Session
class LocalServer : public SocketBase
{
private:
	void OnClosed();

public:
	LocalServer(asio::io_service& serverService);
	~LocalServer();

	void OnPacket(InPacket *iPacket);


	void OnRegisterCenterRequest(InPacket *iPacket);
	void NotifyCenterServerChanged();
};

