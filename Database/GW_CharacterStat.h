#pragma once
#include <string>

struct GW_CharacterStat
{
	int nHP = 0,
		nMP = 0,
		nMaxHP = 0,
		nMaxMP = 0,
		nJob = 0,
		nSubJob = 0,
		nStr = 0,
		nDex = 0,
		nInt = 0,
		nLuk = 0,
		nAP = 0;

	long long int nExp = 0;

	std::string strSP;

public:
	GW_CharacterStat();
	~GW_CharacterStat();

	void Load(int nCharacterID);
	void Save(int nCharacterID, bool isNewCharacter);
};

