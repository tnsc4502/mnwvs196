#include "QWUInventory.h"
#include  "User.h"
#include "Reward.h"
#include "Field.h"
#include "DropPool.h"
#include "SkillInfo.h"
#include "..\Common\Net\InPacket.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "InventoryManipulator.h"

QWUInventory::QWUInventory()
{
}


QWUInventory::~QWUInventory()
{
}

bool QWUInventory::ChangeSlotPosition(User * pUser, int bOnExclRequest, int nTI, int nPOS1, int nPOS2, int nCount, int tRequestTime)
{
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
				auto slotPos = pCharacterData->FindEmptySlotPosition(nTI);
				InventoryManipulator::SwapSlot(pCharacterData, aChangeLog, nTI, nPOS1, nPOS2);
				pUser->OnAvatarModified();
			}
		}
	}
	OutPacket oPacket;
	InventoryManipulator::MakeInventoryOperation(&oPacket, bOnExclRequest, aChangeLog);
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
