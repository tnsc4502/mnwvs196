#pragma once

#include <vector>

class GA_Character;
class OutPacket;
class GW_ItemSlotBase;

class InventoryManipulator
{
public:
	struct ChangeLog
	{
		GW_ItemSlotBase *pItem;
		int nChange, nTI, nPOS, nPOS2, nNumber;
	};

	InventoryManipulator();
	~InventoryManipulator();

	static bool IsItemExist(GA_Character* pCharacterData, int nTI, int nItemID);
	static bool RawAddItem(GA_Character* pCharacterData, int nTI, GW_ItemSlotBase* pItem, std::vector<ChangeLog>& aChangeLog, int *nIncRet);
	static void InsertChangeLog(std::vector<ChangeLog>& aChangeLog, int nChange, int nTI, int nPOS, GW_ItemSlotBase* pi, int nPOS2, int nNumber);
	static void MakeInventoryOperation(OutPacket *oPacket, int bOnExclResult, std::vector<InventoryManipulator::ChangeLog> &aChangeLog);
	static bool RawRemoveItem(GA_Character* pCharacterData, int nTI, int nPOS, int nCount, std::vector<ChangeLog>& aChangeLog, int *nDecRet, GW_ItemSlotBase **ppItemRemoved);
};

