#pragma once
#include <vector>
#include "GW_ItemSlotEquip.h"

class OutPacket;
class InPacket;

struct GW_Avatar
{
	int nHair, nFace, nSkin;
	std::vector<GW_ItemSlotEquip> aHairEquip, aUnseenEquip, aTotemEquip;

#ifdef DBLIB
	void Load(int nCharacterID);
	void Save(int nCharacterID, bool newCharacter = false);
#endif

	void Encode(OutPacket *oPacket);
	void Decode(InPacket* iPacket);
};