#include "ChannelEntry.h"

ChannelEntry::ChannelEntry()
	: pGameSrv(nullptr)
{
}

ChannelEntry::~ChannelEntry()
{
}

void ChannelEntry::SetExternalIP(int ipInDWORD)
{
	nExternalIP = ipInDWORD;
}

void ChannelEntry::SetExternalPort(short port)
{
	nExternalPort = port;
}

int ChannelEntry::GetExternalIP() const
{
	return nExternalIP;
}

short ChannelEntry::GetExternalPort() const
{
	return nExternalPort;
}
