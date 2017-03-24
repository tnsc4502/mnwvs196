#pragma once
#include <atomic>

struct GW_ItemSlotBase
{
	typedef long long int ATOMIC_COUNT_TYPE;

	enum GW_ItemSlotType
	{
		EQUIP,
		CONSUME,
		INSTALL,
		ETC,
		CASH
	};

	enum GW_ItemSlotStatus
	{
		INVENTORY,
		DROPPED,
		SOLD,
	};

	static ATOMIC_COUNT_TYPE InitItemSN(GW_ItemSlotType type);

	static ATOMIC_COUNT_TYPE IncItemSN(GW_ItemSlotType type)
	{
		static std::atomic<ATOMIC_COUNT_TYPE> eqpAtomicCounter = InitItemSN(GW_ItemSlotType::EQUIP);
		static std::atomic<ATOMIC_COUNT_TYPE> conAtomicCounter = InitItemSN(GW_ItemSlotType::CONSUME);
		static std::atomic<ATOMIC_COUNT_TYPE> insAtomicCounter = InitItemSN(GW_ItemSlotType::INSTALL);
		static std::atomic<ATOMIC_COUNT_TYPE> etcAtomicCounter = InitItemSN(GW_ItemSlotType::ETC);
		if (type == GW_ItemSlotType::EQUIP)
			return ++eqpAtomicCounter;
		else if (type == GW_ItemSlotType::CONSUME)
			return ++conAtomicCounter;
		else if (type == GW_ItemSlotType::INSTALL)
			return ++insAtomicCounter;
		else if (type == GW_ItemSlotType::ETC)
			return ++etcAtomicCounter;
	}

	GW_ItemSlotType nType;
	GW_ItemSlotStatus nStatus = GW_ItemSlotStatus::INVENTORY;
	short nPOS = 0;
	int nItemID = 0,
		nAttribute = 0;

	long long int lExpireDate = -1,
				  lItemSN = -1;

public:
	GW_ItemSlotBase();
	~GW_ItemSlotBase();
};

