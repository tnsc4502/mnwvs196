#pragma once
#include <string>
#include <map>

struct PortalScrollItem
{
	std::string sItemName;
	int nItemID;
	std::map<std::string, int> spec;
};

