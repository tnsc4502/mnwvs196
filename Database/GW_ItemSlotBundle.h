#pragma once
#include "GW_ItemSlotBase.h"

struct GW_ItemSlotBundle :
	public GW_ItemSlotBase
{
	int nNumber = 0;

public:
	GW_ItemSlotBundle();
	~GW_ItemSlotBundle();

	void Load(ATOMIC_COUNT_TYPE SN, GW_ItemSlotType type);
	void Save(int nCharacterID, GW_ItemSlotType type);

	void Encode(OutPacket *oPacket) const;
	void RawEncode(OutPacket *oPacket) const;

	void Decode(InPacket *iPacket);
	void RawDecode(InPacket *iPacket); 
	GW_ItemSlotBase * MakeClone();
};

