#include "InventoryManipulator.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "ItemInfo.h"
#include "SkillInfo.h"

#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

InventoryManipulator::InventoryManipulator()
{
}


InventoryManipulator::~InventoryManipulator()
{
}

void InventoryManipulator::SwapSlot(GA_Character* pCharacterData, std::vector<ChangeLog>& aChangeLog, int nTI, int nPOS1, int nPOS2)
{
	auto pItemSrc = pCharacterData->mItemSlot[nTI][nPOS1];
	auto pItemDst = pCharacterData->mItemSlot[nTI][nPOS2];
	pItemSrc->nPOS = nPOS2;
	if(pItemDst)
		pItemDst->nPOS = nPOS1;

	if (pItemDst)
		pCharacterData->mItemSlot[nTI][nPOS1] = pItemDst;
	else
		pCharacterData->mItemSlot[nTI].erase(nPOS1);
	pCharacterData->mItemSlot[nTI][nPOS2] = pItemSrc;
	InventoryManipulator::InsertChangeLog(aChangeLog, 2, nTI, nPOS1, nullptr, nPOS2, 1);
}

bool InventoryManipulator::IsItemExist(GA_Character* pCharacterData, int nTI, int nItemID)
{
	return pCharacterData->GetItemCount(nTI, nItemID) > 0;
}

/*
呼叫方上鎖
*/
bool InventoryManipulator::RawAddItem(GA_Character * pCharacterData, int nTI, GW_ItemSlotBase * pItem, std::vector<ChangeLog>& aChangeLog, int * nIncRet)
{
	/*
	此處檢查是不CashItem
	*/
	if (nTI < 1 || nTI > 5)
		return false;
	auto& itemSlot = pCharacterData->mItemSlot[nTI];
	if (pItem->IsTreatSingly())
	{
		short nPOS = pCharacterData->FindEmptySlotPosition(nTI);
		if (nPOS > 0) 
		{
			itemSlot[nPOS] = pItem;
			pItem->nPOS = nPOS;
			InsertChangeLog(aChangeLog, 0, nTI, nPOS, pItem, 0, 0);
			*nIncRet = 1;
			return true;
		}
		return false;
	}

	if (ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID) != nullptr)
	{
		int nMaxPerSlot = SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(pItem->nItemID, pCharacterData),
			//nLastPos = pCharacterData->mItemSlot[nTI].size() == 0 ? 1 : pCharacterData->mItemSlot[nTI].rbegin()->first,
			nPOS = 1,
			nOnTrading = 0,
			nRemaining = 0,
			nSlotInc = 0,
			nTotalInc = 0,
			nNumber = ((GW_ItemSlotBundle*)pItem)->nNumber; //要加入欄位的物品數量


		//while (nPOS <= nLastPos)
		for(auto& pos : itemSlot)
		{
			nPOS = pos.first;
			if (nPOS <= 0)
				continue;
			if (nPOS >= 10000)
				break;

			auto pItemInSlot = pCharacterData->GetItem(nTI, nPOS);
			
			//先從背包找到相同的物品
			if (pItemInSlot != nullptr && pItemInSlot->nItemID == pItem->nItemID)
			{
				//確認該欄位還可以放多少個相同物品
				nRemaining = (nMaxPerSlot - ((GW_ItemSlotBundle*)pItemInSlot)->nNumber);
				
				//此欄位已滿，繼續找下一個
				if (nRemaining <= 0)
					continue;
				nOnTrading = pCharacterData->mItemTrading[nTI][nPOS];
				nSlotInc = nNumber > nRemaining ? nRemaining : (nNumber); //這次可以增加多少
				//printf("Add To Bag %d, nNumber = %d, nRemaining = %d, nMaxPerSlot = %d\n", nPOS, nNumber, nRemaining, nMaxPerSlot);
				if (nSlotInc - nOnTrading > 0)
				{
					((GW_ItemSlotBundle*)pItemInSlot)->nNumber += (nSlotInc - nOnTrading);
					InsertChangeLog(aChangeLog, 1, nTI, nPOS, pItemInSlot, 0, ((GW_ItemSlotBundle*)pItemInSlot)->nNumber);
				}
				else
				{
					//還不確定mItemTrading是啥
				}
				nNumber -= nSlotInc;
				nTotalInc += nSlotInc;
				if (nNumber <= 0)
					break;
			}

		}

		//欄位無相同物品，找新的欄位插入
		while (nNumber > 0)
		{
			WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "InventoryManipulator::RawAddItem nItemID = %d nNumber = %d nMaxPerSlot = %d Test = %d\n", pItem->nItemID, nNumber, nMaxPerSlot, (int)(ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID) == nullptr));
			nPOS = pCharacterData->FindEmptySlotPosition(nTI);

			//告知物品並未完全放入背包中。
			if (nPOS <= 0)
			{
				*nIncRet = nTotalInc;
				return false;
			}
			/*
			如果pItem剩餘的數量沒有超過nMaxPerSlot，整個pItem放進slot中。
			如果超過最大數量，則產生一個pClone，數量為nMaxPerSlot，並且將pItem剩餘數量減去nSlotInc (nMaxPerSlot)。
			*/
			auto pClone = nNumber > nMaxPerSlot ? pItem->MakeClone() : pItem;
			nSlotInc = nNumber > nMaxPerSlot ? nMaxPerSlot : (nNumber); //這次可以增加多少 ?
			((GW_ItemSlotBundle*)pClone)->nNumber = nSlotInc;
			itemSlot[nPOS] = pClone;
			pClone->nPOS = nPOS;

			InsertChangeLog(aChangeLog, 0, nTI, nPOS, pClone, 0, 0);
			nNumber -= nSlotInc;
			nTotalInc += nSlotInc;
		}
		*nIncRet = nTotalInc;
	}
	return true;
}

/*
呼叫方上鎖
*/
bool InventoryManipulator::RawAddItem(GA_Character * pCharacterData, int nTI, int nItemID, int nCount, std::vector<ChangeLog>& aChangeLog, int * nIncRet)
{
	/*
	此處檢查是不CashItem
	*/
	if (nTI < 1 || nTI > 5)
		return false;
	auto& itemSlot = pCharacterData->mItemSlot[nTI];
	auto pItem = ItemInfo::GetInstance()->GetItemSlot(nItemID, ItemInfo::ITEMVARIATION_NORMAL);
	if (pItem->IsTreatSingly())
	{
		short nPOS = pCharacterData->FindEmptySlotPosition(nTI);
		if (nPOS > 0)
		{
			itemSlot[nPOS] = pItem;
			pItem->nPOS = nPOS;
			InsertChangeLog(aChangeLog, 0, nTI, nPOS, pItem, 0, 0);
			*nIncRet = 1;
			return true;
		}
		return false;
	}

	if (ItemInfo::GetInstance()->GetBundleItem(nItemID) != nullptr)
	{
		int nMaxPerSlot = SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(nItemID, pCharacterData),
			//nLastPos = pCharacterData->mItemSlot[nTI].size() == 0 ? 1 : pCharacterData->mItemSlot[nTI].rbegin()->first,
			nPOS = 1,
			nOnTrading = 0,
			nRemaining = 0,
			nSlotInc = 0,
			nTotalInc = 0,
			nNumber = nCount;

		for (auto& pos : itemSlot)
		{
			nPOS = pos.first;
			if (nPOS <= 0)
				continue;
			if (nPOS >= 10000)
				break;

			auto pItemInSlot = pCharacterData->GetItem(nTI, nPOS);

			//先從背包找到相同的物品
			if (pItemInSlot != nullptr && pItemInSlot->nItemID == nItemID)
			{
				//確認該欄位還可以放多少個相同物品
				nRemaining = (nMaxPerSlot - ((GW_ItemSlotBundle*)pItemInSlot)->nNumber);

				//此欄位已滿，繼續找下一個
				if (nRemaining <= 0)
					continue;
				nOnTrading = pCharacterData->mItemTrading[nTI][nPOS];
				nSlotInc = nNumber > nRemaining ? nRemaining : (nNumber); //這次可以增加多少
																		  //printf("Add To Bag %d, nNumber = %d, nRemaining = %d, nMaxPerSlot = %d\n", nPOS, nNumber, nRemaining, nMaxPerSlot);
				if (nSlotInc - nOnTrading > 0)
				{
					((GW_ItemSlotBundle*)pItemInSlot)->nNumber += (nSlotInc - nOnTrading);
					InsertChangeLog(aChangeLog, 1, nTI, nPOS, pItemInSlot, 0, ((GW_ItemSlotBundle*)pItemInSlot)->nNumber);
				}
				else
				{
					//還不確定mItemTrading是啥
				}
				nNumber -= nSlotInc;
				nTotalInc += nSlotInc;
				if (nNumber <= 0)
					break;
			}

		}

		//欄位無相同物品，找新的欄位插入
		while (nNumber > 0)
		{
			WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "InventoryManipulator::RawAddItem nItemID = %d nNumber = %d nMaxPerSlot = %d Test = %d\n", pItem->nItemID, nNumber, nMaxPerSlot, (int)(ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID) == nullptr));
			nPOS = pCharacterData->FindEmptySlotPosition(nTI);

			//告知物品並未完全放入背包中。
			if (nPOS <= 0)
			{
				*nIncRet = nTotalInc;
				return false;
			}
			/*
			如果pItem剩餘的數量沒有超過nMaxPerSlot，整個pItem放進slot中。
			如果超過最大數量，則產生一個pClone，數量為nMaxPerSlot，並且將pItem剩餘數量減去nSlotInc (nMaxPerSlot)。
			*/
			auto pClone = nNumber > nMaxPerSlot ? pItem->MakeClone() : pItem;
			nSlotInc = nNumber > nMaxPerSlot ? nMaxPerSlot : (nNumber); //這次可以增加多少 ?
			((GW_ItemSlotBundle*)pClone)->nNumber = nSlotInc;
			itemSlot[nPOS] = pClone;
			pClone->nPOS = nPOS;

			InsertChangeLog(aChangeLog, 0, nTI, nPOS, pClone, 0, 0);
			nNumber -= nSlotInc;
			nTotalInc += nSlotInc;
		}
		*nIncRet = nTotalInc;
	}
	return true;
}

void InventoryManipulator::InsertChangeLog(std::vector<ChangeLog>& aChangeLog, int nChange, int nTI, int nPOS, GW_ItemSlotBase * pi, int nPOS2, int nNumber)
{
	ChangeLog newLog;
	newLog.nChange = nChange;
	newLog.nTI = nTI;
	newLog.nPOS = nPOS;
	newLog.nPOS2 = nPOS2;
	newLog.nNumber = nNumber;
	newLog.pItem = pi;
	aChangeLog.push_back(newLog);
}

void InventoryManipulator::MakeInventoryOperation(OutPacket * oPacket, int bOnExclResult, std::vector<InventoryManipulator::ChangeLog>& aChangeLog)
{
	oPacket->Encode2((short)UserSendPacketFlag::UserLocal_OnInventoryOperation);
	oPacket->Encode1(bOnExclResult);
	oPacket->Encode1((char)aChangeLog.size());
	oPacket->Encode1(0);
	for (auto& change : aChangeLog)
	{
		//printf("Encoding Inventory Operation\n");
		oPacket->Encode1((char)change.nChange);
		oPacket->Encode1((char)change.nTI);
		oPacket->Encode2((short)change.nPOS);
		if (change.nChange)
		{
			if (change.nChange == 1)
				oPacket->Encode2((short)change.nNumber);
			if (change.nChange == 2)
				oPacket->Encode2((short)change.nPOS2);
			if (change.nChange == 3 && change.nPOS < 0)
				oPacket->Encode1(1);
			oPacket->Encode1(0);
		}
		else 
		{
			change.pItem->RawEncode(oPacket);
			//oPacket->Encode4(0); // what's this?
		}
	}
	//printf("Encoding Inventory Operation Done\n");
	oPacket->Encode4(0); // what's this?
}

/*
呼叫方上鎖
*/
bool InventoryManipulator::RawRemoveItem(GA_Character * pCharacterData, int nTI, int nPOS, int nCount, std::vector<ChangeLog>& aChangeLog, int * nDecRet, GW_ItemSlotBase ** ppItemRemoved)
{
	auto pItem = pCharacterData->GetItem(nTI, nPOS);
	GW_ItemSlotBase* pClone = ppItemRemoved ? pItem->MakeClone() : nullptr;
	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "Raw Remove Item pClone == null ? %d\n", (int)(pClone == nullptr));
	if (pItem != nullptr)
	{
		int nRemaining = 0;
		//
		if (pItem->IsTreatSingly() && nTI == 1)
			pCharacterData->RemoveItem(nTI, nPOS);
		else if (nCount >= 1)
		{
			GW_ItemSlotBundle* pBundle = (GW_ItemSlotBundle*)pItem;
			int nInSlotCount = pBundle->nNumber;
			if (nCount > nInSlotCount)
				nCount = nInSlotCount;
			pBundle->nNumber -= nCount;
			nRemaining = pBundle->nNumber;
			if (nRemaining <= 0)
				pCharacterData->RemoveItem(nTI, nPOS);
		}

		if (ppItemRemoved) 
		{
			*ppItemRemoved = pClone;
		}
		if (ppItemRemoved && nCount >= 1 && nTI != 1)
			((GW_ItemSlotBundle*)*ppItemRemoved)->nNumber = nCount;

		*nDecRet = nCount;
		if(nRemaining > 0)
			InsertChangeLog(aChangeLog, 1, nTI, nPOS, pItem, 0, nRemaining);
		else
			InsertChangeLog(aChangeLog, 3, nTI, nPOS, pItem, 0, *nDecRet);
	}
	return true;
}
