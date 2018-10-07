#pragma once
#include <string>
#include "BasicItemStat.h"

struct UpgradeItem
{
	std::string sItemName;
	BasicIncrementStat incStat;
	int nItemID,
		nSuccessRate,
		nCursedRate;
};

