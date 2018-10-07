#pragma once
#include <string>
#include <vector>

struct PetFoodItem
{
	std::string sItemName;
	int nItemID,
		niRepleteness;
	std::vector<int> ldwPet;
};

