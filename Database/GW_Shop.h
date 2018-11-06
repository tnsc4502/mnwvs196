#pragma once
#include <map>
#include <vector>
#include "..\WvsLib\Common\CommonDef.h"

struct GW_Shop
{
	ALLOW_PRIVATE_ALLOC
public:
	struct ShopItem
	{
		int nNpcID = 0,
			nItemID = 0,
			nPrice = 0,
			nTokenItemID = 0,
			nTokenPrice = 0,
			nQuantity = 0,
			nMaxPerSlot = 0,
			nTabIndex = 0,
			nLevelLimited = 0,
			nShowLevMin = 0,
			nShowLevMax = 0;

		long long int lilPeriod = 0, liSellStart = 0, liSellEnd = 0;
	};

private:
	std::map<int, std::vector<ShopItem*>> m_mNpcShop;

	GW_Shop();
	~GW_Shop();

public:

	static GW_Shop* GetInstance();
	std::vector<ShopItem*>* GetShopItemList(int nNpcID);
	void Load();
};

