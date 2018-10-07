#pragma once
#include <string>
#include <vector>

struct SkillLearnItem
{
	std::string sItemName;
	int nItemID,
		nMasterLevel,
		nSuccessRate,
		nReqLevel;

	std::vector<int> aSkill;
};

