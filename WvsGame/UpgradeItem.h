#pragma once
#include <string>
#include "BasicItemStat.h"

struct UpgradeItem
{
	std::string sItemName;
	BasicIncrementStat incStat;
	int nItemID = 0,
		nSuccessRate = 0,
		nCursedRate = 0;
};

