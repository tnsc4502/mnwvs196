#pragma once
#include <string>
#include <list>

namespace WvsLoginConstants
{
	const int kMaxNumberOfCenters = 30;

	struct CenterServerInfo
	{
		std::string strServerIP;
		short nServerPort;
		short nServerID;
	};
	
	const CenterServerInfo CenterServerList[] = {
		{ "127.0.0.1", 8383, 1 },
		{ "127.0.0.1", 8384, 2 },
	};
}