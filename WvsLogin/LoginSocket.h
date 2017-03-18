#pragma once
#include "Net\SocketBase.h"

class LoginSocket :
	public SocketBase
{
private:
	void OnClosed();
public:
	LoginSocket(asio::io_service& serverService);
	~LoginSocket();

	void OnPacket(InPacket *iPacket);

	void OnClientRequestStart();
	void OnLoginBackgroundRequest();
};

