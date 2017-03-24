#pragma once
#include "GW_ItemSlotBase.h"
struct GW_ItemSlotEquip :
	public GW_ItemSlotBase
{
	int nRUC = 0, 
		nCUC = 0, 
		nI_STR = 0, 
		nI_DEX = 0, 
		nI_INT = 0, 
		nI_LUK = 0, 
		nI_MaxHP = 0, 
		nI_MaxMP = 0, 
		nI_PAD = 0, 
		nI_MAD = 0, 
		nI_PDD = 0, 
		nI_MDD = 0, 
		nI_ACC = 0, 
		nI_EVA = 0, 
		nI_Speed = 0, 
		nI_Craft = 0, 
		nI_Jump = 0;
public:
	GW_ItemSlotEquip();
	~GW_ItemSlotEquip();

	void Load(int SN);
	void Save(int nCharacterID);
};

