#pragma once
#include <string>
#include "BasicItemStat.h"

struct BundleItem
{
	std::string sItemName;
	double dSellUnitPrice;
	int nItemID,
		nPAD,
		nMAD,
		nRequiredLEV,
		nSellPrice,
		nMaxPerSlot;

	BasicAbilityStat abilityStat;
};
