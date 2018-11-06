#include "Npc.h"
#include "NpcTemplate.h"
#include "User.h"
#include "ExchangeElement.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "QWUInventory.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"

void Npc::OnShopPurchaseItem(User * pUser, InPacket * iPacket)
{
	int nPOS = iPacket->Decode2();
	int nItemID = iPacket->Decode4();
	int nCount = iPacket->Decode2();
	iPacket->Decode4();
	int nUnitPrice = iPacket->Decode4();

	auto& aItemList = *(m_pTemplate->GetShopItem());
	if (nPOS >= 0 && nPOS < aItemList.size())
	{
		auto pItem = aItemList[nPOS];

		if (nCount > 0 && pItem)
		{
			int nPrice = pItem->nPrice * (ItemInfo::IsRechargable(nItemID) ? 1 : nCount);
			std::vector<InventoryManipulator::ChangeLog> aChangeLog;
			std::vector<ExchangeElement> aExchange;
			nCount *= pItem->nQuantity;
			if (pItem->nTokenPrice > 0)
			{
				ExchangeElement token;
				token.m_nItemID = pItem->nTokenItemID;
				token.m_nCount = -(pItem->nTokenPrice);
				aExchange.push_back(token);
			}
			ExchangeElement shopItem;
			shopItem.m_nItemID = pItem->nItemID;
			shopItem.m_nCount = nCount;
			aExchange.push_back(shopItem);

			auto nResult = QWUInventory::Exchange(
				pUser,
				-nPrice,
				aExchange,
				aChangeLog,
				aChangeLog
			);
			switch (nResult)
			{
			case InventoryManipulator::Exchange_InsufficientMeso:
				pUser->SendNoticeMessage(0, "楓幣不足。");
				break;
			case InventoryManipulator::Exchange_InsufficientSlotCount:
				pUser->SendNoticeMessage(0, "道具欄位不足。");
				break;
			case InventoryManipulator::Exchange_InsufficientItemCount:
				pUser->SendNoticeMessage(0, "需求道具數量不足。");
				break;
			}
			OutPacket oPacket;
			MakeShopResult(pUser, pItem, &oPacket, 0, -1);
			pUser->SendPacket(&oPacket);
		}
	}
}

void Npc::OnShopSellItem(User * pUser, InPacket * iPacket)
{
	int nPOS = iPacket->Decode2();
	int nItemID = iPacket->Decode4();
	int nCount = iPacket->Decode2();
	int nTI = ItemInfo::GetItemSlotType(nItemID);
	int nPrice = 0;
	if (nTI > 0 && nTI <= 5 && nCount > 0)
	{
		auto pItem = pUser->GetCharacterData()->GetItem(
			nTI,
			nPOS
		);
		if (pItem == nullptr)
		{
			pUser->SendNoticeMessage(0, "物品不存在。");
			return;
		}
		if (nTI == 1) 
		{
			nCount = 1;
			auto pEquipItem = ItemInfo::GetInstance()->GetEquipItem(nItemID);
			if (pEquipItem)
				nPrice = nCount * pEquipItem->nSellPrice;
		}
		else
		{
			auto pBundle = (GW_ItemSlotBundle*)pItem;
			auto pBundleItem = ItemInfo::GetInstance()->GetBundleItem(nItemID);
			nCount = (nCount > pBundle->nNumber ? pBundle->nNumber : nCount);
			if (pBundleItem && ItemInfo::IsRechargable(nItemID))
				nPrice = (int)std::ceil(nCount * pBundleItem->dSellUnitPrice);
			else if (pBundleItem)
					nPrice = nCount * pBundleItem->nSellPrice;
		}
		if (nCount > 0)
		{
			auto pSoldItem = pItem->MakeClone();
			std::vector<InventoryManipulator::ChangeLog> aChangeLog;
			std::vector<ExchangeElement> aExchange;

			ExchangeElement shopItem;
			shopItem.m_pItem = pItem;
			shopItem.m_nCount = -nCount;
			aExchange.push_back(shopItem);

			auto nResult = QWUInventory::Exchange(
				pUser,
				nPrice,
				aExchange,
				aChangeLog,
				aChangeLog
			);
			if (!nResult)
				(m_pTemplate->GetUserSoldItems(pUser->GetUserID())->push_back(pSoldItem));
			OutPacket oPacket;
			MakeShopResult(pUser, pItem, &oPacket, 0, -1);
			pUser->SendPacket(&oPacket);
		}
	}
}

void Npc::OnShopRechargeItem(User * pUser, InPacket * iPacket)
{
}

void Npc::MakeShopResult(User *pUser, void* pItem_, OutPacket * oPacket, int nAction, int nIdx)
{
	GW_Shop::ShopItem* pItem = (GW_Shop::ShopItem*)pItem_;
	oPacket->Encode2((short)UserSendPacketFlag::User_OnNpcShopRequest);
	oPacket->Encode1(nAction);
	if (nAction == 8)
	{
		oPacket->Encode4(0);
		oPacket->Encode4(GetTemplateID());
		oPacket->Encode4(0); //nStarCoin
		oPacket->Encode1(0);
		oPacket->Encode4(0); //nShopVerNo

		oPacket->Encode1(0); //Rank stuffs
		oPacket->Encode4(0);
		oPacket->Encode2(1);
		NpcTemplate::EncodeShopItem(
			pUser, pItem, oPacket
		);
	}
	else
	{
		oPacket->Encode1(nIdx >= 0 ? 1 : 0);
		if (nIdx >= 0)
			oPacket->Encode4(nIdx);
		else
		{
			oPacket->Encode1(0);
			oPacket->Encode4(0);
		}
		oPacket->Encode4(0);
	}
}

Npc::Npc()
{
}

Npc::~Npc()
{
}

void Npc::SetTemplate(NpcTemplate * pTemplate)
{
	m_pTemplate = pTemplate;
}

NpcTemplate * Npc::GetTemplate()
{
	return m_pTemplate;
}

void Npc::OnShopRequest(User * pUser, InPacket * iPacket)
{
	char nAction = iPacket->Decode1();
	switch (nAction)
	{
		case 0:
			OnShopPurchaseItem(pUser, iPacket);
			break;
		case 1:
			OnShopSellItem(pUser, iPacket);
			break;
		case 2:
			OnShopRechargeItem(pUser, iPacket);
			break;
		default:
			pUser->SetTradingNpc(nullptr);
			break;
	}
}

void Npc::OnUpdateLimitedInfo(User * pUser, InPacket * iPacket)
{
	int nRemained = iPacket->RemainedCount();
	OutPacket oPacket;
	oPacket.Encode2(0x417);
	//printf("[LifePool::OnNpcPacket][OnUpdateLimitedInfo]Remained = %d\n", nRemained);
	if (nRemained == 6)
	{
		oPacket.Encode4(GetFieldObjectID());
		oPacket.Encode2(iPacket->Decode2());
		oPacket.Encode4(iPacket->Decode4());
	}
	else
		return;
	pUser->SendPacket(&oPacket);
}

void Npc::SendChangeControllerPacket(User * pUser)
{
	OutPacket oPacket;
	oPacket.Encode2(0x415);
	oPacket.Encode1(1);
	oPacket.Encode4(GetFieldObjectID());
	oPacket.Encode4(GetTemplateID());
	EncodeInitData(&oPacket);
	pUser->SendPacket(&oPacket);
}

void Npc::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2(0x412); //CNpcPool::OnUserEnterField
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode4(GetTemplateID());
	EncodeInitData(oPacket);
}

void Npc::MakeLeaveFieldPacket(OutPacket * oPacket)
{
}

void Npc::EncodeInitData(OutPacket *oPacket)
{
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetCy());
	oPacket->Encode1(0);
	oPacket->Encode1(~GetF());
	oPacket->Encode2(GetFh());
	oPacket->Encode2(GetRx0());
	oPacket->Encode2(GetRx1());
	oPacket->Encode1(!IsHidden());
	oPacket->Encode4(0);
	oPacket->Encode1(0);
	oPacket->Encode4(-1);
	oPacket->Encode4(0);

	oPacket->Encode4(0);
	oPacket->Encode4(0);

	oPacket->EncodeStr("");
	oPacket->Encode1(0);
}