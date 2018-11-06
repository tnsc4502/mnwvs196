#pragma once
#include <atomic>
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

struct GW_ItemSlotBundle;
struct GW_ItemSlotEquip;
struct GW_ItemSlotPet;

struct GW_ItemSlotBase
{
	typedef long long int ATOMIC_COUNT_TYPE;
	static const int LOCK_POS = 32767;

	enum GW_ItemSlotType
	{
		EQUIP = 1,
		CONSUME,
		INSTALL,
		ETC,
		CASH
	};

	enum GW_ItemSlotInstanceType
	{
		GW_ItemSlotEquip_Type = 1,
		GW_ItemSlotBundle_Type = 2,
		GW_ItemSlotPet_Type = 3
	};

	static ATOMIC_COUNT_TYPE InitItemSN(GW_ItemSlotType type);
	static ATOMIC_COUNT_TYPE IncItemSN(GW_ItemSlotType type);

	GW_ItemSlotType nType;
	short nPOS = 0;

	int nItemID = 0,
		nAttribute = 0,
		nCharacterID = -1;

	long long int liExpireDate = GameDateTime::TIME_UNLIMITED,
				  liItemSN = -1,
				  liCashItemSN = -1;

	bool bIsInBag = false, bIsPet = false, bIsCash =  false;
	unsigned char nInstanceType = 0;

public:
	GW_ItemSlotBase();
	~GW_ItemSlotBase();

	virtual void Encode(OutPacket *oPacket, bool bForInternal) const = 0;
	virtual void RawEncode(OutPacket *oPacket) const = 0;

	void DecodeInventoryPosition(InPacket *iPacket);
	virtual void Decode(InPacket *iPacket, bool bForInternal) = 0;
	virtual void RawDecode(InPacket *iPacket) = 0;

	void EncodeInventoryPosition(OutPacket *oPacket) const;
	void EncodeTradingPosition(OutPacket *oPacket) const;

	virtual GW_ItemSlotBase* MakeClone() = 0;

	virtual void Load(ATOMIC_COUNT_TYPE SN) = 0;
	virtual void Save(int nCharacterID) = 0;

	static GW_ItemSlotBase* CreateItem(int nIntanceType);
	virtual void Release() = 0;
};

#define FreeItemSlot(pItem) \
if (pItem->nType == GW_ItemSlotBase::EQUIP)\
	FreeObj((GW_ItemSlotEquip*)pItem);\
else if (pItem->bIsPet)\
	FreeObj((GW_ItemSlotPet*)pItem);\
else\
	FreeObj((GW_ItemSlotBundle*)pItem);