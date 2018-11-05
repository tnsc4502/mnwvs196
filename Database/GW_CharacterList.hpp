#pragma once
#include <vector>

struct GW_CharacterList
{
	int nCount;
	std::vector<int> aCharacterList;

#ifdef DBLIB
	void Load(int nAccountID, int nWorldID);
#endif

};