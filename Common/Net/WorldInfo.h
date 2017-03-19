#pragma once
#include <string>

struct WorldConnectionInfo
{
	std::string strServerIP;
	short nServerPort;
};

struct WorldInfo
{
	int nWorldID, nEventType, nGameCount;
	std::string strWorldDesc, strEventDesc;
};