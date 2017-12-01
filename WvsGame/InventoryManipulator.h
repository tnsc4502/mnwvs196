#pragma once

#include <vector>

class GA_Character;
class GW_ItemSlotBase;

class InventoryManipulator
{
	struct ChangeLog
	{
		int nChange, nTI, nPOS, nPOS2, nNumber;
	};
public:
	InventoryManipulator();
	~InventoryManipulator();

	bool IsItemExist(GA_Character* pCharacterData, int nTI, int nItemID);
	bool RawAddItem(GA_Character* pCharacterData, int nTI, GW_ItemSlotBase* pItem, std::vector<ChangeLog>& aChangeLog, int *nIncRet);
	void InsertChangeLog(std::vector<ChangeLog>& aChangeLog, int nChange, int nTI, int nPOS, GW_ItemSlotBase* pi, int nPOS2, int nNumber);
};

