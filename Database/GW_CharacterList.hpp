#pragma once
#include <vector>

struct GW_CharacterList
{
	int nCount;
	std::vector<int> aCharacterList;

	void Load(int nAccountID, int nWorldID);
};