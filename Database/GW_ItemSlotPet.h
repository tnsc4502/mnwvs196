#pragma once
#include "GW_ItemSlotBase.h"
#include <string>

struct GW_ItemSlotPet : public GW_ItemSlotBase
{
	std::string strPetName;

	unsigned char nLevel = 0,
				  nRepleteness = 0,
				  nActiveState = 0;

	short nTameness = 0,
		  nPetAttribute = 0,
		  nGiantRate = 0;

	int nRemainLife = 0,
		nPetHue = 0,
		nAutoBuffSkill = 0;

	unsigned short usPetSkill = 0;

	GW_ItemSlotPet();
	~GW_ItemSlotPet();

	void Load(ATOMIC_COUNT_TYPE SN);
	void Save(int nCharacterID);

	void Encode(OutPacket *oPacket, bool bForInternal) const;
	void RawEncode(OutPacket *oPacket) const ;

	void Decode(InPacket *iPacket, bool bForInternal);
	void RawDecode(InPacket *iPacket);
	void Release();

	GW_ItemSlotBase* MakeClone();
};

