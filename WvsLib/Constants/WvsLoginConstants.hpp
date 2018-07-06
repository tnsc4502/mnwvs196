#pragma once
#include <string>
#include <list>
#include "..\WvsLib\Net\WorldInfo.h"

namespace WvsLoginConstants
{
	const int kMaxNumberOfCenters = 30;
	
	const WorldConnectionInfo CenterServerList[2] = {
		WorldConnectionInfo{ "127.0.0.1", 8383 },
		WorldConnectionInfo{ "127.0.0.1", 8384 }
	};
}