#pragma once
#include "Net\SocketBase.h"

//Server ºÝ¤§ Session
class ServerBase : public SocketBase
{
private:
	void OnClosed();
	unsigned char nServerType;

public:
	ServerBase(asio::io_service& serverService);
	~ServerBase();

	void OnPacket(InPacket *iPacket);

	void SetServerType(unsigned char type)
	{
		nServerType = type;
	}

	void OnRegisterCenterRequest(InPacket *iPacket);
};

