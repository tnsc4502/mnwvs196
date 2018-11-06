#include "QWUInventory.h"
#include  "User.h"
#include "Reward.h"
#include "Field.h"
#include "DropPool.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_CharacterSlotCount.h"
#include "BasicStat.h"

QWUInventory::QWUInventory()
{
}


QWUInventory::~QWUInventory()
{
}

bool QWUInventory::ChangeSlotPosition(User * pUser, int bOnExclRequest, int nTI, int nPOS1, int nPOS2, int nCount, int tRequestTime)
{
	std::lock_guard<std::mutex> lock(pUser->GetLock());
	auto pCharacterData = pUser->GetCharacterData();
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	int nMovedCount = 0;
	if (pCharacterData->mStat->nHP && nPOS1 != nPOS2)
	{
		//將物品從背包移除並且在地圖上嘗試召喚
		if (nPOS2 == 0)
		{
			GW_ItemSlotBase* pItemCopyed = nullptr;
			InventoryManipulator::RawRemoveItem(pCharacterData, nTI, nPOS1, nCount, aChangeLog, &nMovedCount, &pItemCopyed);
			if (pItemCopyed && pUser->GetField())
			{
				Reward reward;
				reward.SetType(1);
				reward.SetItem(pItemCopyed);
				pUser->GetField()->GetDropPool()->Create(
					&reward,
					pUser->GetUserID(),
					0,
					0,
					0,
					pUser->GetPosX(),
					pUser->GetPosY(),
					pUser->GetPosX(),
					pUser->GetPosY(),
					0,
					0,
					0,
					0);
			}
		}
		else if (nPOS1 > 0 && nPOS2 > 0) //在背包內移動
		{
			auto pItemSrc = pCharacterData->GetItem(nTI, nPOS1);
			auto pItemDst = pCharacterData->GetItem(nTI, nPOS2);
			//Equip
			if (nTI == 1)
				InventoryManipulator::SwapSlot(pCharacterData, aChangeLog, nTI, nPOS1, nPOS2);
			else
			{
				if (pItemDst != nullptr && pItemSrc->nItemID == pItemDst->nItemID)
				{
					int nMaxPerSlot = SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(pItemSrc->nItemID, pCharacterData);
					int nSlotSrcCount = ((GW_ItemSlotBundle*)pItemSrc)->nNumber;
					int nSlotDstCount = ((GW_ItemSlotBundle*)pItemDst)->nNumber;
					if (nSlotSrcCount + nSlotDstCount <= nMaxPerSlot)
					{
						InventoryManipulator::RawRemoveItem(pCharacterData, nTI, nPOS1, nSlotSrcCount, aChangeLog, &nMovedCount, nullptr);
						((GW_ItemSlotBundle*)pItemDst)->nNumber += nSlotSrcCount;
						InventoryManipulator::InsertChangeLog(aChangeLog, 1, nTI, nPOS2, nullptr, 0, ((GW_ItemSlotBundle*)pItemDst)->nNumber);
					}
					else
					{
						((GW_ItemSlotBundle*)pItemDst)->nNumber = nMaxPerSlot;
						((GW_ItemSlotBundle*)pItemSrc)->nNumber = nMaxPerSlot = nSlotSrcCount + nSlotDstCount - nMaxPerSlot;
						InventoryManipulator::InsertChangeLog(aChangeLog, 1, nTI, nPOS1, nullptr, 0, ((GW_ItemSlotBundle*)pItemSrc)->nNumber);
						InventoryManipulator::InsertChangeLog(aChangeLog, 1, nTI, nPOS2, nullptr, 0, ((GW_ItemSlotBundle*)pItemDst)->nNumber);
					}
				}
				else
					InventoryManipulator::SwapSlot(pCharacterData, aChangeLog, nTI, nPOS1, nPOS2);
			}
		}
		else if ((nPOS1 < 0 && nPOS2 > 0) || nPOS2 < 0) //脫下裝備
		{
			auto pItemSrc = pCharacterData->GetItem(nTI, nPOS1);
			auto pItemDst = pCharacterData->GetItem(nTI, nPOS2);
			if (nTI == 1)
			{
				if (pItemSrc->nAttribute & ItemInfo::ItemAttribute::eTradeBlockAfterEquip) 
				{
					pItemSrc->nAttribute &= (~ItemInfo::ItemAttribute::eTradeBlockAfterEquip);
					pItemSrc->nAttribute |= (ItemInfo::ItemAttribute::eUntradable);
					InventoryManipulator::InsertChangeLog(aChangeLog, 3, nTI, nPOS1, nullptr, nPOS2, 1);
					InventoryManipulator::InsertChangeLog(aChangeLog, 0, nTI, nPOS1, pItemSrc, nPOS2, 1);
				}
				auto slotPos = pCharacterData->FindEmptySlotPosition(nTI);
				InventoryManipulator::SwapSlot(pCharacterData, aChangeLog, nTI, nPOS1, nPOS2);
				pUser->OnAvatarModified();
			}
		}
	}
	OutPacket oPacket;
	InventoryManipulator::MakeInventoryOperation(&oPacket, bOnExclRequest, aChangeLog);
	pUser->SendCharacterStat(true, 0);
	pUser->SendPacket(&oPacket);
	return false;
}

void QWUInventory::OnChangeSlotPositionRequest(User * pUser, InPacket * iPacket)
{

	//0xF7 0x00 [0x71 0xF2 0x4A 0x52] [0x01] [0xF9 0xFF] [0x01 0x00] [0xFF 0xFF]
	int tRequestTime = iPacket->Decode4();
	char nTI = iPacket->Decode1();
	short nPOS1 = iPacket->Decode2();
	short nPOS2 = iPacket->Decode2();
	int nCount = iPacket->Decode2();
	ChangeSlotPosition(pUser, true, nTI, nPOS1, nPOS2, nCount, tRequestTime);
}

bool QWUInventory::PickUpMoney(User* pUser, bool byPet, int nAmount)
{
	std::lock_guard<std::mutex> lock(pUser->GetLock());
	InventoryManipulator::RawIncMoney(pUser->GetCharacterData(), nAmount);

	auto liFlag = BasicStat::BasicStatFlag::BS_Meso;
	pUser->SendCharacterStat(true, liFlag);
	return true;
}

bool QWUInventory::PickUpItem(User * pUser, bool byPet, GW_ItemSlotBase * pItem)
{
	//printf("Pick Up Item Requert\n");
	std::lock_guard<std::mutex> lock(pUser->GetLock());
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	int totalInc = 0;
	bool result = false;
	if(pItem != nullptr)
		result = InventoryManipulator::RawAddItem(
			pUser->GetCharacterData(),
			pItem->nType,
			pItem,
			aChangeLog,
			&totalInc,
			true
		);	
	OutPacket oPacket;
	InventoryManipulator::MakeInventoryOperation(&oPacket, true, aChangeLog);
	pUser->SendPacket(&oPacket);
	pUser->SendCharacterStat(true, 0);
	return result;
}

/*
此處對pUser上鎖
*/
bool QWUInventory::RawRemoveItemByID(User * pUser, int nItemID, int nCount)
{
	std::lock_guard<std::mutex> lock(pUser->GetLock());
	auto pCharacterData = pUser->GetCharacterData();
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	while (nCount > 0)
	{
		auto pItem = pCharacterData->GetItemByID(nItemID);
		if (pItem == nullptr)
			break;
		int nRemovedAtCurrentSlot = 0;
		InventoryManipulator::RawRemoveItem(pCharacterData, pItem->nType, pItem->nPOS, nCount, aChangeLog, &nRemovedAtCurrentSlot, nullptr);
		nCount -= nRemovedAtCurrentSlot;
	}

	OutPacket oPacket;
	InventoryManipulator::MakeInventoryOperation(&oPacket, true, aChangeLog);
	pUser->SendPacket(&oPacket);
	pUser->SendCharacterStat(true, 0);
	return nCount == 0;
}

bool QWUInventory::RawRemoveItem(User * pUser, int nTI, int nPOS, int nCount, std::vector<InventoryManipulator::ChangeLog>& aChangeLog, int & nDecRet, GW_ItemSlotBase ** ppItemRemoved)
{
	std::lock_guard<std::mutex> lock(pUser->GetLock());
	if (pUser->GetCharacterData()->mStat->nHP != 0)
		return InventoryManipulator::RawRemoveItem(
			pUser->GetCharacterData(),
			nTI,
			nPOS,
			nCount,
			aChangeLog,
			&nDecRet,
			ppItemRemoved
		);
	return false;
}

/*
此處對pUser上鎖
*/
bool QWUInventory::RawAddItemByID(User * pUser, int nItemID, int nCount)
{
	std::lock_guard<std::mutex> lock(pUser->GetLock());
	auto pCharacterData = pUser->GetCharacterData();
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	int nTotalAdded = 0;
	while (nTotalAdded < nCount)
	{
		int nAddedAtCurrentSlot = 0;
		
		InventoryManipulator::RawAddItem(pCharacterData, nItemID / 1000000, nItemID, nCount, aChangeLog, &nAddedAtCurrentSlot);
		nTotalAdded += nAddedAtCurrentSlot;
	}

	OutPacket oPacket;
	InventoryManipulator::MakeInventoryOperation(&oPacket, true, aChangeLog);
	pUser->SendPacket(&oPacket);
	pUser->SendCharacterStat(true, 0);
	return nTotalAdded == nCount;
}

int QWUInventory::Exchange(User * pUser, int nMoney, std::vector<ExchangeElement>& aExchange, std::vector<InventoryManipulator::ChangeLog>& aLogAdd, std::vector<InventoryManipulator::ChangeLog>& aLogRemove)
{
	std::lock_guard<std::mutex> lock(pUser->GetLock());
	int nResult = InventoryManipulator::RawExchange(
		pUser->GetCharacterData(),
		nMoney,
		aExchange,
		aLogAdd,
		aLogRemove
	);
	if (!nResult)
	{
		if (nMoney)
			pUser->SendCharacterStat(true, BasicStat::BasicStatFlag::BS_Meso);

		QWUInventory::SendInventoryOperation(pUser, false, aLogRemove);
		QWUInventory::SendInventoryOperation(pUser, false, aLogAdd);
	}
	return nResult;
}

void QWUInventory::SendInventoryOperation(User* pUser, int bOnExclResult, std::vector<InventoryManipulator::ChangeLog>& aChangeLog)
{
	OutPacket oPacket;
	InventoryManipulator::MakeInventoryOperation(&oPacket, bOnExclResult, aChangeLog);
	pUser->SendPacket(&oPacket);
}

int QWUInventory::GetSlotCount(User * pUser, int nTI)
{
	if (nTI < 1 || nTI > 5)
		return 0;
	std::lock_guard<std::mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mSlotCount->aSlotCount[nTI];
}

int QWUInventory::GetHoldCount(User * pUser, int nTI)
{
	if (nTI < 1 || nTI > 5)
		return 0;
	int nMaxSlotCount = GetSlotCount(pUser, nTI);
	std::lock_guard<std::mutex> lock(pUser->GetLock());
	auto& mSlot = pUser->GetCharacterData()->mItemSlot[nTI];
	int nRet = 0;
	for (auto& pItem : mSlot)
		if (pItem.second->nPOS >= 1 && pItem.second->nPOS <= nMaxSlotCount)
			++nRet;
	return nRet;
}

int QWUInventory::GetFreeCount(User * pUser, int nTI)
{
	if (nTI < 1 || nTI > 5)
		return 0;
	int nMaxSlotCount = GetSlotCount(pUser, nTI);
	int nHoldCount = GetHoldCount(pUser, nTI);
	return nMaxSlotCount - nHoldCount;
}
