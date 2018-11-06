#pragma once
#include <map>
#include <vector>
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\Database\GW_Shop.h"

class User;
struct GW_ItemSlotBase;
class OutPacket;

class NpcTemplate
{
	int m_nTemplateID = 0;
	static std::map<int, NpcTemplate*> m_mNpcTemplates;
	static std::map<int, std::vector<GW_ItemSlotBase*>> m_mUserSoldItem;

	std::vector<GW_Shop::ShopItem*>* m_aShopItem = nullptr;

public:
	NpcTemplate();
	~NpcTemplate();

	void Load();
	void RegisterNpc(int nNpcID, void *pProp);
	NpcTemplate* GetNpcTemplate(int dwTemplateID);
	bool HasShop() const;
	static NpcTemplate* GetInstance();

	std::vector<GW_Shop::ShopItem*>* GetShopItem();
	void EncodeShop(User *pUser, OutPacket *oPacket);
	static void EncodeShopItem(User *pUser, GW_Shop::ShopItem* pItem, OutPacket *oPacket);

	static std::vector<GW_ItemSlotBase*>* GetUserSoldItems(int nUserID);
	static void ClearUserSoldItems(int nUserID);
};

