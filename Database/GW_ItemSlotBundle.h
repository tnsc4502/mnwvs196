#pragma once
#include "GW_ItemSlotBase.h"

struct GW_ItemSlotBundle :
	public GW_ItemSlotBase
{
	int nNumber = 0;

public:
	GW_ItemSlotBundle();
	~GW_ItemSlotBundle();

	void Load(ATOMIC_COUNT_TYPE SN);
	void Save(int nCharacterID);

	void Encode(OutPacket *oPacket, bool bForInternal) const;
	void RawEncode(OutPacket *oPacket) const;

	void Decode(InPacket *iPacket, bool bForInternal);
	void RawDecode(InPacket *iPacket); 

	void Release();
	GW_ItemSlotBase * MakeClone();
};

