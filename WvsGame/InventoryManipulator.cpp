#include "InventoryManipulator.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotBase.h"

InventoryManipulator::InventoryManipulator()
{
}


InventoryManipulator::~InventoryManipulator()
{
}

bool InventoryManipulator::IsItemExist(GA_Character* pCharacterData, int nTI, int nItemID)
{
	return pCharacterData->GetItemCount(nTI, nItemID) > 0;
}

bool InventoryManipulator::RawAddItem(GA_Character * pCharacterData, int nTI, GW_ItemSlotBase * pItem, std::vector<ChangeLog>& aChangeLog, int * nIncRet)
{
	/*
	此處檢查是不CashItem
	*/
	auto& itemSlot = pCharacterData->mItemSlot[nTI];
	if (pItem->IsTreatSingly())
	{
		short nPOS = pCharacterData->FindEmptySlotPosition(nTI);
		if (nPOS > 0) 
		{
			pCharacterData->mItemSlot[nTI][nPOS] = pItem;
			InsertChangeLog(aChangeLog, 0, nTI, nPOS, pItem, 0, 0);
		}

	}
}

void InventoryManipulator::InsertChangeLog(std::vector<ChangeLog>& aChangeLog, int nChange, int nTI, int nPOS, GW_ItemSlotBase * pi, int nPOS2, int nNumber)
{
	ChangeLog newLog;
	newLog.nChange = nChange;
	newLog.nTI = nTI;
	newLog.nPOS = nPOS;
	newLog.nPOS2 = nPOS2;
	newLog.nNumber = nNumber;
	aChangeLog.push_back(newLog);
}
