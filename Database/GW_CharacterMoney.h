#pragma once

struct GW_CharacterMoney
{
	long long int nMoney = 0;
public:
	GW_CharacterMoney();
	~GW_CharacterMoney();

	void Load(int nCharacterID);
	void Save(int nCharacterID, bool bIsNewCharacter = false);
};

