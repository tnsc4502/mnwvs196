#pragma once
#include <vector>
#include <string>
#include <map>
#include "TemporaryStat.h"

class User;

struct StateChangeItem
{
	std::string sItemName;
	int nItemID;
	std::map<std::string, int> spec;

	TemporaryStat::TS_Flag Apply(User* pUser, int tCur, bool bApplyBetterOnly, bool bResetByItem = false, bool bForcedSetTime = false, int nForcedSetTime = 0);
};

