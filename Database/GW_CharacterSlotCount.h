#pragma once
struct GW_CharacterSlotCount
{
	int aSlotCount[6];

	void Load(int nCharacterID);
	void Save(int nCharacterID, bool bIsNewCharacter = false);
};

