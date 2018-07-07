#include "LocalServerEntry.h"

LocalServerEntry::LocalServerEntry()
	: pLocalSocket(nullptr)
{
}

LocalServerEntry::~LocalServerEntry()
{
}

void LocalServerEntry::SetExternalIP(int ipInDWORD)
{
	nExternalIP = ipInDWORD;
}

void LocalServerEntry::SetExternalPort(short port)
{
	nExternalPort = port;
}

int LocalServerEntry::GetExternalIP() const
{
	return nExternalIP;
}

short LocalServerEntry::GetExternalPort() const
{
	return nExternalPort;
}
