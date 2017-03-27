#pragma once
#include "LocalServer.h"
class ChannelEntry
{
private:
	std::shared_ptr<SocketBase> pGameSrv;
	int nMaxUser, nTotalUser, nExternalIP;
	short nExternalPort;

public:
	ChannelEntry();
	~ChannelEntry();

	void SetExternalIP(int ipInDWORD);
	void SetExternalPort(short port);

	int GetExternalIP() const;
	short GetExternalPort() const;

	void SetGameServer(std::shared_ptr<SocketBase> &srv)
	{
		pGameSrv = srv;
	}

	std::shared_ptr<SocketBase> &GetGameServer()
	{
		return pGameSrv;
	}
};

