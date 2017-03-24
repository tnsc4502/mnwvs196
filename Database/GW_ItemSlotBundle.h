#pragma once
#include "GW_ItemSlotBase.h"

struct GW_ItemSlotBundle :
	public GW_ItemSlotBase
{
	int nNumber = 0;

public:
	GW_ItemSlotBundle();
	~GW_ItemSlotBundle();

	void Load(int SN, const char* strTableName);
	void Save(int nCharacterID, GW_ItemSlotType type);
};

