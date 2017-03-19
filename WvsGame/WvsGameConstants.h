#pragma once
#include "Net\WorldInfo.h"

namespace WvsGameConstants
{
	static int nGamePort;

	static std::string strGameDesc;

	static WorldConnectionInfo CenterServerList[1] =
	{
		WorldConnectionInfo{ "127.0.0.1", 8383 }
	};
}