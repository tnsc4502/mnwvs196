#pragma once
#include "Net\SocketBase.h"

class ClientSocket :
	public SocketBase
{
	void OnClosed();

public:
	ClientSocket(asio::io_service& serverService);
	~ClientSocket();

	void OnPacket(InPacket *iPacket);
};

