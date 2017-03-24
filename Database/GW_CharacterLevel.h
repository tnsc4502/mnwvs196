#pragma once

struct GW_CharacterLevel
{
	unsigned char nLevel = 1;

public:
	GW_CharacterLevel();
	~GW_CharacterLevel();

	void Load(int nCharacterID);
	void Save(int nCharacterID, bool isNewCharacter = false);
};

