#pragma once

struct GW_CharacterLevel
{
	unsigned char nLevel = 1;

public:
	GW_CharacterLevel();
	~GW_CharacterLevel();

#ifdef DBLIB
	void Load(int nCharacterID);
	void Save(int nCharacterID, bool bNewCharacter = false);
#endif

};

