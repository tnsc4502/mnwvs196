#include "InventoryManipulator.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterSlotCount.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_CharacterMoney.h"
#include "BackupItem.h"
#include "ExchangeElement.h"
#include "ItemInfo.h"
#include "SkillInfo.h"

#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

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
	InventoryManipulator::InsertChangeLog(aChangeLog, ChangeType::Change_SlotPOSChanged, nTI, nPOS1, nullptr, nPOS2, 1);
}

bool InventoryManipulator::IsItemExist(GA_Character* pCharacterData, int nTI, int nItemID)
{
	return pCharacterData->GetItemCount(nTI, nItemID) > 0;
}

bool InventoryManipulator::RawIncMoney(GA_Character * pCharacterData, int nMoney)
{
	if(pCharacterData->mMoney->nMoney + nMoney < 0)
		return false;
	pCharacterData->mMoney->nMoney += nMoney;
	return true;
}

/*
呼叫方上鎖
*/
bool InventoryManipulator::RawAddItem(GA_Character * pCharacterData, int nTI, GW_ItemSlotBase * pItem, std::vector<ChangeLog>& aChangeLog, int * nIncRet, bool bDeleteIfItemCombined, std::vector<BackupItem>* paBackupItem)
{
	/*
	此處檢查是不CashItem
	*/
	if (nTI < GW_ItemSlotBase::EQUIP || nTI > GW_ItemSlotBase::CASH)
		return false;
	auto& itemSlot = pCharacterData->mItemSlot[nTI];
	if (ItemInfo::IsTreatSingly(pItem->nItemID, pItem->liExpireDate))
	{
		short nPOS = pCharacterData->FindEmptySlotPosition(nTI);
		if (nPOS > 0) 
		{
			if (pItem->liItemSN != -1)
				pCharacterData->mItemRemovedRecord[nTI].erase(pItem->liItemSN);
			itemSlot[nPOS] = pItem;
			pItem->nPOS = nPOS;
			InsertChangeLog(aChangeLog, ChangeType::Change_AddToSlot, nTI, nPOS, pItem, 0, 0);
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
			if (nPOS > pCharacterData->mSlotCount->aSlotCount[nTI])
				break;

			auto pItemInSlot = (GW_ItemSlotBundle*)pCharacterData->GetItem(nTI, nPOS);
			
			//先從背包找到相同的物品
			if (!ItemInfo::IsRechargable(pItem->nItemID) && 
				pItemInSlot != nullptr && 
				pItemInSlot->nItemID == pItem->nItemID)
			{
				//確認該欄位還可以放多少個相同物品
				nRemaining = (nMaxPerSlot - pItemInSlot->nNumber);
				
				//此欄位已滿，繼續找下一個
				if (nRemaining <= 0)
					continue;
				nOnTrading = pCharacterData->mItemTrading[nTI][nPOS];
				nSlotInc = nNumber > nRemaining ? nRemaining : (nNumber); //這次可以增加多少
				//printf("Add To Bag %d, nNumber = %d, nRemaining = %d, nMaxPerSlot = %d\n", nPOS, nNumber, nRemaining, nMaxPerSlot);
				if (nSlotInc - nOnTrading > 0)
				{
					if (paBackupItem) 
					{
						auto pBackup = pItemInSlot->MakeClone();
						pBackup->liCashItemSN = pItemInSlot->liCashItemSN;
						pBackup->liItemSN = pItemInSlot->liItemSN;
						(*paBackupItem).push_back({ nTI, nPOS,  pBackup });
					}
					pItemInSlot->nNumber += (nSlotInc - nOnTrading);
					InsertChangeLog(aChangeLog, ChangeType::Change_QuantityChanged, nTI, nPOS, pItemInSlot, 0, pItemInSlot->nNumber);
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

		//物品完全合併
		if (nNumber == 0 && bDeleteIfItemCombined)
			FreeObj((GW_ItemSlotBundle*)pItem);

		//欄位無相同物品，找新的欄位插入
		while (nNumber > 0)
		{
			WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "InventoryManipulator::RawAddItem nItemID = %d nNumber = %d nMaxPerSlot = %d Test = %d\n", pItem->nItemID, nNumber, nMaxPerSlot, (int)(ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID) == nullptr));
			nPOS = pCharacterData->FindEmptySlotPosition(nTI);

			//告知物品並未完全放入背包中。
			if (nPOS <= 0)
			{
				((GW_ItemSlotBundle*)pItem)->nNumber = nNumber;
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

			if(pClone->liItemSN != -1)
				pCharacterData->mItemRemovedRecord[nTI].erase(pClone->liItemSN);
			if (paBackupItem)
				(*paBackupItem).push_back({ nTI, nPOS, nullptr });
			itemSlot[nPOS] = pClone;
			pClone->nPOS = nPOS;

			InsertChangeLog(aChangeLog, ChangeType::Change_AddToSlot, nTI, nPOS, pClone, 0, 0);
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
bool InventoryManipulator::RawAddItem(GA_Character * pCharacterData, int nTI, int nItemID, int nCount, std::vector<ChangeLog>& aChangeLog, int * nIncRet, std::vector<BackupItem>* paBackupItem)
{
	/*
	此處檢查是不CashItem
	*/
	if (nTI < GW_ItemSlotBase::EQUIP || nTI > GW_ItemSlotBase::CASH)
		return false;
	auto pItem = ItemInfo::GetInstance()->GetItemSlot(nItemID, ItemInfo::ITEMVARIATION_NORMAL);
	if (pItem == nullptr)
		return false;
	if (nTI != GW_ItemSlotBase::EQUIP)
		((GW_ItemSlotBundle*)pItem)->nNumber = nCount;
	return RawAddItem(pCharacterData, nTI, pItem, aChangeLog, nIncRet, true, paBackupItem);
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
			if (change.nChange == ChangeType::Change_QuantityChanged)
				oPacket->Encode2((short)change.nNumber);
			if (change.nChange == ChangeType::Change_SlotPOSChanged)
				oPacket->Encode2((short)change.nPOS2);
			//if (change.nChange == 3 && change.nPOS < 0);
				//oPacket->Encode1(0);
			oPacket->Encode1(0);
		}
		else 
		{
			change.pItem->RawEncode(oPacket);
			oPacket->Encode1(0); // what's this?
		}
	}
	//printf("Encoding Inventory Operation Done\n");
	oPacket->Encode4(0); // what's this?
}

void InventoryManipulator::MakeItemUpgradeEffect(OutPacket *oPacket, int nCharacterID, int nEItemID, int nUItemID, bool bSuccess, bool bCursed, bool bEnchant)
{
	oPacket->Encode2((short)UserSendPacketFlag::UserCommon_ShowItemUpgradeEffect);
	oPacket->Encode4(nCharacterID);
	oPacket->Encode1(bSuccess ? 1 : (bCursed ? 2 : 0));
	oPacket->Encode1(bEnchant);
	oPacket->Encode4(nUItemID);
	oPacket->Encode4(nEItemID);
	oPacket->Encode4(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
}

/*
呼叫方上鎖
*/
bool InventoryManipulator::RawRemoveItem(GA_Character * pCharacterData, int nTI, int nPOS, int nCount, std::vector<ChangeLog>& aChangeLog, int * nDecRet, GW_ItemSlotBase ** ppItemRemoved, std::vector<BackupItem>* paBackupItem)
{
	auto pItem = pCharacterData->GetItem(nTI, nPOS);
	GW_ItemSlotBase* pClone = ppItemRemoved ? pItem->MakeClone() : nullptr;
	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "Raw Remove Item pClone == null ? %d\n", (int)(pClone == nullptr));
	bool bCountSufficient = true;
	long long int liItemSN = -1, liCashItemSN = -1;
	if (pItem != nullptr)
	{
		liItemSN = pItem->liItemSN;
		liCashItemSN = pItem->liCashItemSN;
		if (paBackupItem) 
		{
			auto pBackup = pItem->MakeClone();
			pBackup->liCashItemSN = liCashItemSN;
			pBackup->liItemSN = liItemSN;
			(*paBackupItem).push_back({ nTI, nPOS, pBackup });
		}
		int nRemaining = 0;
		//
		if (ItemInfo::IsTreatSingly(pItem->nItemID, pItem->liExpireDate) && nTI == 1)
			pCharacterData->RemoveItem(nTI, nPOS);
		else if (nCount >= 1)
		{
			GW_ItemSlotBundle* pBundle = (GW_ItemSlotBundle*)pItem;
			int nInSlotCount = pBundle->nNumber;
			if (ItemInfo::IsRechargable(pItem->nItemID))
				nCount = pBundle->nNumber;

			if (nCount > nInSlotCount)
			{
				bCountSufficient = false;
				nCount = nInSlotCount;
			}

			pBundle->nNumber -= nCount;
			nRemaining = pBundle->nNumber;
			if (nRemaining <= 0)
				pCharacterData->RemoveItem(nTI, nPOS);
		}

		//複製一個實體，如果是整個物品移出，SN也要一起改
		if (ppItemRemoved) 
		{
			(*ppItemRemoved) = pClone;
			if (nRemaining <= 0) 
			{
				(*ppItemRemoved)->liItemSN = liItemSN;
				(*ppItemRemoved)->liCashItemSN = liCashItemSN;
			}
		}

		if (ppItemRemoved && nCount >= 1 && nTI != GW_ItemSlotBase::EQUIP) 
			((GW_ItemSlotBundle*)*ppItemRemoved)->nNumber = nCount;

		*nDecRet = nCount;
		if (nRemaining > 0)
			InsertChangeLog(aChangeLog, ChangeType::Change_QuantityChanged, nTI, nPOS, pItem, 0, nRemaining);
		else
			InsertChangeLog(aChangeLog, ChangeType::Change_RemoveFromSlot, nTI, nPOS, pItem, 0, *nDecRet);
	}
	else
		return false;
	return bCountSufficient;
}

int InventoryManipulator::RawExchange(GA_Character * pCharacterData, int nMoney, std::vector<ExchangeElement>& aExchange, std::vector<ChangeLog>& aLogAdd, std::vector<ChangeLog>& aLogRemove)
{
	std::vector<BackupItem> aBackupItem;
	int nDel = 0, nAdd = 0;
	for (auto& elem : aExchange)
	{
		if (elem.m_nCount < 0)
		{
			int nCount = elem.m_nCount * -1;
			while (nCount > 0)
			{
				int nRemovedAtCurrentSlot = 0;
				if (elem.m_pItem)
				{
					RawRemoveItem(
						pCharacterData,
						elem.m_pItem->nType,
						elem.m_pItem->nPOS,
						nCount,
						aLogRemove,
						&nRemovedAtCurrentSlot, 
						nullptr, 
						&aBackupItem);
				}
				else
				{
					auto pItem = pCharacterData->GetItemByID(elem.m_nItemID);
					if (pItem == nullptr)
						break;
					RawRemoveItem(
						pCharacterData, 
						pItem->nType, 
						pItem->nPOS, 
						nCount, 
						aLogRemove, 
						&nRemovedAtCurrentSlot, 
						nullptr, 
						&aBackupItem);
				}
				nCount -= nRemovedAtCurrentSlot;
			}
			if(nCount != 0)
			{
				RestoreBackupItem(pCharacterData, &aBackupItem);
				return Exchange_InsufficientItemCount;
			}
		}
		else
		{
			if ((elem.m_pItem != nullptr && !RawAddItem(pCharacterData, elem.m_pItem->nType, elem.m_pItem, aLogAdd, &nAdd, true, &aBackupItem))
				|| (!RawAddItem(pCharacterData, elem.m_nItemID / 1000000, elem.m_nItemID, elem.m_nCount, aLogAdd, &nAdd, &aBackupItem)))
			{
				RestoreBackupItem(pCharacterData, &aBackupItem);
				return Exchange_InsufficientSlotCount;
			}
		}
	}
	if (!RawIncMoney(pCharacterData, nMoney))
	{
		RestoreBackupItem(pCharacterData, &aBackupItem);
		return Exchange_InsufficientMeso;
	}

	for (auto& pBackup : aBackupItem)
		if(pBackup.m_pItem)
			pBackup.m_pItem->Release();

	return Exchange_Success;
}

void InventoryManipulator::RestoreBackupItem(GA_Character * pCharacterData, std::vector<BackupItem>* paBackupItem)
{
	for (auto& pItemBackup : (*paBackupItem))
	{
		auto pItem = pCharacterData->GetItem(pItemBackup.m_nTI, pItemBackup.m_nPOS);
		if (pItemBackup.m_pItem == nullptr)
			pCharacterData->mItemSlot[pItemBackup.m_nTI].erase(pItemBackup.m_nPOS);
		else 
		{
			pCharacterData->SetItem(pItemBackup.m_nTI, pItemBackup.m_nPOS, pItemBackup.m_pItem);
			pCharacterData->mItemRemovedRecord[pItemBackup.m_nTI].erase(pItemBackup.m_pItem->liItemSN);
		}
		if (pItem != nullptr)
			pItem->Release();
	}
}
