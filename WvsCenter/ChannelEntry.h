#pragma once
#include "LocalServer.h"
class ChannelEntry
{
private:
	std::shared_ptr<SocketBase> pGameSrv;
	int nMaxUser, nTotalUser;

public:
	ChannelEntry();
	~ChannelEntry();

	void SetGameServer(std::shared_ptr<SocketBase> &srv)
	{
		pGameSrv = srv;
	}

	std::shared_ptr<SocketBase> &GetGameServer()
	{
		return pGameSrv;
	}
};

