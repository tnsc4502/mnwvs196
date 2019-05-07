#pragma once
#include <map>
#include "GW_ItemSlotEquip.h"

class OutPacket;
class InPacket;

struct GW_Avatar
{
	int nHair, nFace, nSkin;

	//		nPOS, nItemID
	std::map<short, int> mEquip, mUnseenEquip, mTotemEquip;

#ifdef DBLIB
	void Load(int nCharacterID);
	void Save(int nCharacterID, bool newCharacter = false);
#endif

	void Encode(OutPacket *oPacket);
	void Decode(InPacket* iPacket);
};