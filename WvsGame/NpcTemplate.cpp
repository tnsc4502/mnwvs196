#include "NpcTemplate.h"
#include "User.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

std::map<int, NpcTemplate*> NpcTemplate::m_mNpcTemplates;
std::map<int, std::vector<GW_ItemSlotBase*>> NpcTemplate::m_mUserSoldItem;

NpcTemplate::NpcTemplate()
{
}


NpcTemplate::~NpcTemplate()
{
}

void NpcTemplate::Load()
{
	auto& refWz = stWzResMan->GetWz(Wz::Npc);
	for (auto& refNpc : refWz)
	{
		RegisterNpc(atoi(refNpc.Name().c_str()), &refNpc);
	}
}

void NpcTemplate::RegisterNpc(int nNpcID, void * pProp)
{
	NpcTemplate *pTemplate = AllocObj(NpcTemplate);
	pTemplate->m_nTemplateID = nNpcID;
	pTemplate->m_aShopItem = GW_Shop::GetInstance()->GetShopItemList(nNpcID);
	m_mNpcTemplates[nNpcID] = pTemplate;
}

NpcTemplate* NpcTemplate::GetNpcTemplate(int dwTemplateID)
{
	auto findIter = m_mNpcTemplates.find(dwTemplateID);
	if (findIter == m_mNpcTemplates.end())
		return nullptr;
	return findIter->second;
}

bool NpcTemplate::HasShop() const
{
	return m_aShopItem != nullptr;
}

NpcTemplate * NpcTemplate::GetInstance()
{
	static NpcTemplate* pInstance = AllocObj(NpcTemplate);
	return pInstance;
}

std::vector<GW_Shop::ShopItem*>* NpcTemplate::GetShopItem()
{
	return m_aShopItem;
}

void NpcTemplate::EncodeShop(User * pUser, OutPacket * oPacket)
{
	oPacket->Encode4(0); //nSelectNpcItemID
	oPacket->Encode4(m_nTemplateID);
	oPacket->Encode4(0); //nStarCoin
	oPacket->Encode1(0);
	oPacket->Encode4(0); //nShopVerNo

	oPacket->Encode1(0); //Rank stuffs
	oPacket->Encode4(0);
	oPacket->Encode2((short)(!m_aShopItem ? 0 : m_aShopItem->size()));
	if (m_aShopItem)
	{
		for (auto& item : *m_aShopItem)
			EncodeShopItem(pUser, item, oPacket);
	}
}

void NpcTemplate::EncodeShopItem(User *pUser, GW_Shop::ShopItem * pItem, OutPacket * oPacket)
{
	oPacket->Encode4(pItem->nItemID);
	oPacket->Encode4(pItem->nTokenItemID);
	oPacket->Encode4(pItem->nTokenPrice);
	oPacket->Encode4(0); //nPointQuestID
	oPacket->Encode4(pItem->nPrice);
	oPacket->Encode1(0); //dDiscount
	oPacket->Encode4(0); //nPointPrice
	oPacket->Encode4(0); //nStarCoin
	oPacket->Encode4(0); //nQuestExID
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(1440 * (int)pItem->lilPeriod);
	oPacket->Encode4(pItem->nLevelLimited);
	oPacket->Encode2(pItem->nShowLevMin);
	oPacket->Encode2(pItem->nShowLevMax);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode8(pItem->liSellStart);
	oPacket->Encode8(pItem->liSellEnd);
	oPacket->Encode4(pItem->nTabIndex);
	oPacket->Encode4(0);
	oPacket->EncodeStr("");
	oPacket->Encode4(pItem->nTabIndex);
	oPacket->EncodeStr("0000000000");
	oPacket->EncodeStr("9999123100");

	if (!ItemInfo::IsRechargable(pItem->nItemID)) 
	{
		oPacket->Encode2(pItem->nQuantity);
		oPacket->Encode2(pItem->nMaxPerSlot);
	}
	else
	{
		//oPacket->EncodeStr("333333");
		auto pBundleItem = ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID);
		double dPrice = 0;
		if (pBundleItem)
		{
			dPrice = pBundleItem->dSellUnitPrice;
			oPacket->EncodeBuffer((unsigned char*)&dPrice, 8);
		}
		oPacket->Encode2(
			SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(
				pItem->nItemID,
				pUser->GetCharacterData()
			));
	}

	for (int i = 0; i < 4; ++i)
		oPacket->Encode4(0);

	for (int i = 0; i < 5; ++i)
	{
		if (i == 4)
			oPacket->Encode8(0);
		else
		{
			oPacket->Encode4(9410165 + i);
			oPacket->Encode4(0);
		}
	}
	oPacket->Encode1(0);
}

std::vector<GW_ItemSlotBase*>* NpcTemplate::GetUserSoldItems(int nUserID)
{
	return &(m_mUserSoldItem[nUserID]);
}

void NpcTemplate::ClearUserSoldItems(int nUserID)
{
	auto findIter = m_mUserSoldItem.find(nUserID);
	if (findIter != m_mUserSoldItem.end())
		m_mUserSoldItem.erase(findIter);
}
