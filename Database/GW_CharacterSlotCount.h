#pragma once
struct GW_CharacterSlotCount
{
	int aSlotCount[5];

	void Load(int nCharacterID);
	void Save(int nCharacterID, bool bIsNewCharacter = false);
};

