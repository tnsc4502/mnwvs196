#pragma once
#include <string>

struct WorldConnectionInfo
{
	std::string strServerIP;
	short nServerPort;
};

struct WorldInfo
{
	int m_aChannelStatus[30];
	int nWorldID, nEventType, nGameCount;
	std::string strWorldDesc, strEventDesc;
};