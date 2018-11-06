#include "GW_Shop.h"
#include "WvsUnified.h"
#include "..\WvsGame\ItemInfo.h"
#include "..\WvsLib\Wz\WzResMan.hpp"


GW_Shop::GW_Shop()
{
}


GW_Shop::~GW_Shop()
{
}

void GW_Shop::Load()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM Shop Order by NpcID, SN";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	for (auto& result : recordSet)
	{
		ShopItem *pItem = AllocObj(ShopItem);
		int nNpcID = (int)result["NpcID"];
		pItem->nNpcID = nNpcID;
		pItem->nItemID = (int)result["ItemID"];
		pItem->nPrice = (int)result["Price"];
		pItem->nTokenItemID = (int)result["TokenItemID"];
		pItem->nTokenPrice = (int)result["TokenPrice"];
		pItem->nQuantity = (int)result["Quantity"];
		pItem->nMaxPerSlot = (int)result["MaxPerSlot"];
		pItem->nTabIndex = (int)result["TabIndex"];
		pItem->nLevelLimited = (int)result["LevelLimited"];
		pItem->lilPeriod = (long long int)result["Period"];
		pItem->nShowLevMin = (int)result["ShowLevMin"];
		pItem->nShowLevMax = (int)result["ShowLevMax"];
		pItem->liSellStart = (long long int)result["ftSellStart"];
		pItem->liSellEnd = (long long int)result["ftSellEnd"];
		if(ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID) != nullptr
			|| ItemInfo::GetInstance()->GetEquipItem(pItem->nItemID) != nullptr)
			m_mNpcShop[nNpcID].push_back(pItem);
	}
}

GW_Shop * GW_Shop::GetInstance()
{
	static GW_Shop* pInstance = AllocObj(GW_Shop);
	return pInstance;
}

std::vector<GW_Shop::ShopItem*>* GW_Shop::GetShopItemList(int nNpcID)
{
	auto findIter = m_mNpcShop.find(nNpcID);
	if (findIter == m_mNpcShop.end())
		return nullptr;
	return &(findIter->second);
}
