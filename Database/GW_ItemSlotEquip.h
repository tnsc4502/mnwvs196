#pragma once
#include "GW_ItemSlotBase.h"
struct GW_ItemSlotEquip :
	public GW_ItemSlotBase
{
	struct ItemOption
	{

	};

	enum EquipFlag
	{
		EQP_RUC				 = 0x1,
		EQP_CUC				 = 0x2,
		EQP_STR				 = 0x4,
		EQP_DEX				 = 0x8,
		EQP_INT				 = 0x10,
		EQP_LUK				 = 0x20,
		EQP_MaxHP			 = 0x40,
		EQP_MaxMP			 = 0x80,
		EQP_PAD				 = 0x100,
		EQP_MAD				 = 0x200,
		EQP_PDD				 = 0x400,
		EQP_Craft			 = 0x4000,
		EQP_Speed			 = 0x8000,
		EQP_Jump			 = 0x10000, 
		EQP_Attribute        = 0x20000,
		EQP_LevelUpType      = 0x40000,
		EQP_Level            = 0x80000,
		EQP_EXP64            = 0x100000,
		EQP_Durability       = 0x200000,
		EQP_IUC              = 0x400000,
		EQP_PVPDamage        = 0x800000,
		EQP_ReduceReq        = 0x1000000,
		EQP_SpecialAttribute = 0x2000000,
		EQP_DurabilityMax    = 0x4000000,
		EQP_IncReq           = 0x8000000,
		EQP_GrowthEnchant    = 0x10000000,
		EQP_PSEnchant        = 0x20000000,
		EQP_BDR              = 0x40000000,
		EQP_IMDR             = 0x80000000,

		EQP_DamR             = 0x1,
		EQP_StatR            = 0x2,
		EQP_Cuttable         = 0x4,
		EQP_ExGradeOption    = 0x8,
		EQP_ItemState        = 0x10,
	};

	long long int 
		nEXP64 = 0,
		nExGradeOption = 0;

	int 
		nDurability = 0,
		nIUC = 0,
		nDurabilityMax = 0;

	unsigned short
		nSTR = 0,
		nDEX = 0,
		nINT = 0,
		nLUK = 0,
		nMaxHP = 0,
		nMaxMP = 0,
		nPAD = 0,
		nMAD = 0,
		nPDD = 0,
		nMDD = 0,
		nACC = 0,
		nEVA = 0,
		nSpeed = 0,
		nCraft = 0,
		nJump = 0,
		nLevelUpType = 0,
		nLevel = 0,
		nPVPDamage = 0,
		nSpecialAttribute = 0;

	unsigned char
		nRUC = 0,
		nCUC = 0,
		nReduceReq = 0,
		nIncEeq = 0,
		nGrowthEnchant = 0,
		nPSEnchant = 0,
		nBDR = 0,
		nIMDR = 0,
		nDamR = 0,
		nStatR = 0,
		nCuttable = 0;

	ItemOption* m_pItemOption = nullptr;

public:
	GW_ItemSlotEquip();
	~GW_ItemSlotEquip();

	void Load(ATOMIC_COUNT_TYPE SN);
	void Save(int nCharacterID);

	void Encode(OutPacket *oPacket, bool bForInternal) const;
	void RawEncode(OutPacket *oPacket) const;

	void Decode(InPacket *iPacket, bool bForInternal);
	void RawDecode(InPacket *iPacket);

	void EncodeEquipBase(OutPacket *oPacket) const;
	void EncodeEquipAdvanced(OutPacket *oPacket) const;

	void DecodeEquipBase(InPacket *iPacket);
	void DecodeEquipAdvanced(InPacket *iPacket); 

	void Release();
	GW_ItemSlotBase* MakeClone();
};

