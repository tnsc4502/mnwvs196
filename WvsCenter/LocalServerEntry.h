#pragma once
#include "LocalServer.h"
class LocalServerEntry
{
private:
	std::shared_ptr<SocketBase> pLocalSocket;
	int nMaxUser, nTotalUser, nExternalIP;
	short nExternalPort;

public:
	LocalServerEntry();
	~LocalServerEntry();

	void SetExternalIP(int ipInDWORD);
	void SetExternalPort(short port);

	int GetExternalIP() const;
	short GetExternalPort() const;

	void SetLocalSocket(std::shared_ptr<SocketBase> &pSocket)
	{
		pLocalSocket = pSocket;
	}

	std::shared_ptr<SocketBase> &GetLocalSocket()
	{
		return pLocalSocket;
	}
};

