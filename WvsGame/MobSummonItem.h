#pragma once
#include <string>
#include <map>

struct MobSummonItem
{
	std::string sItemName;
	int nItemID,
		nType;

	// pair < MobID, Prob >
	std::map<int, int> lMob;
};

