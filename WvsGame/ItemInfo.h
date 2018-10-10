#pragma once
#include "StateChangeItem.h"
#include "EquipItem.h"
#include "BundleItem.h"
#include "UpgradeItem.h"
#include "PortalScrollItem.h"
#include "PetFoodItem.h"
#include "MobSummonItem.h"
#include "TamingMobFoodItem.h"
#include "BridleItem.h"
#include "SkillLearnItem.h"
#include "PortableChairItem.h"

#include <vector>

class User;
struct GW_ItemSlotBase;

class ItemInfo
{
private:
	bool m_bInitialized = false;

public:
	enum ItemAttribute
	{
		eNotSale = 0x10,
		eExpireOnLogout = 0x20,
		ePickUpBlock = 0x40,
		eOnly = 0x80,
		eAccountSharable = 0x100,
		eQuest = 0x200,
		eTradeBlock = 0x400,
		eAccountShareTag = 0x800,
		eMobHP = 0x1000
	};

	enum ItemVariationOption
	{
		ITEMVARIATION_NONE, 
		ITEMVARIATION_BETTER,
		ITEMVARIATION_NORMAL,
		ITEMVARIATION_GREAT,
		ITEMVARIATION_GACHAPON
	};

	ItemInfo();
	~ItemInfo();

	static ItemInfo* GetInstance();
	void Initialize();
	void IterateMapString();
	void IterateItemString(void *dataNode);
	void IterateEquipItem(void *dataNode);
	void IterateBundleItem();
	void IteratePetItem();
	void IterateCashItem();
	void RegisterSpecificItems();
	void RegisterNoRollbackItem();
	void RegisterSetHalloweenItem();

	void RegisterEquipItemInfo(EquipItem* pEqpItem, int nItemID, void* pProp);

	/*
	一般升級用卷軸
	*/
	void RegisterUpgradeItem(int nItemID, void *pProp);

	/*
	移動卷軸
	*/
	void RegisterPortalScrollItem(int nItemID, void *pProp);

	/*
	召喚袋之類的
	*/
	void RegisterMobSummonItem(int nItemID, void *pProp);

	/*
	寵物相關的
	*/
	void RegisterPetFoodItem(int nItemID, void *pProp);

	/*
	騎寵相關的
	*/
	void RegisterTamingMobFoodItem(int nItemID, void *pProp);

	/*
	
	*/
	void RegisterBridleItem(int nItemID, void *pProp);

	/*
	椅子
	*/
	void RegisterPortableChairItem(int nItemID, void *pProp);

	/*
	技能書
	*/
	void RegisterSkillLearnItem(int nItemID, void *pProp);

	/*
	會改變角色狀態的物品，例如藥水
	*/
	void RegisterStateChangeItem(int nItemID, void *pProp);

	ItemInfo::EquipItem* GetEquipItem(int nItemID);
	ItemInfo::BundleItem* GetBundleItem(int nItemID);
	ItemInfo::UpgradeItem* GetUpgradeItem(int nItemID);
	ItemInfo::PortalScrollItem* GetPortalScrollItem(int nItemID);

	ItemInfo::MobSummonItem* GetMobSummonItem(int nItemID);
	ItemInfo::PetFoodItem* GetPetFoodItem(int nItemID);
	ItemInfo::TamingMobFoodItem* GetTamingMobFoodItem(int nItemID);
	ItemInfo::BridleItem* GetBridleItem(int nItemID);
	ItemInfo::SkillLearnItem* GetSkillLearnItem(int nItemID);
	ItemInfo::PortableChairItem* GetPortableChairItem(int nItemID);
	ItemInfo::StateChangeItem* GetStateChangeItem(int nItemID);

	bool ConsumeOnPickup(int nItemID);
	bool ExpireOnLogout(int nItemID);
	int GetBulletPAD(int nItemID);
	long long int GetItemDateExpire(const std::string& sDate);
	const std::string& GetItemName(int nItemID);
	bool IsAbleToEquip(int nGender, int nLevel, int nJob, int nSTR, int nDEX, int nINT, int nLUK, int nPOP, GW_ItemSlotBase* pPetItem, int nItemID);
	bool IsNotSaleItem(int nItemID);
	bool IsOnlyItem(int nItemID);
	bool IsTradeBlockItem(int nItemID);
	bool IsQuestItem(int nItemID);
	GW_ItemSlotBase* GetItemSlot(int nItemID, ItemVariationOption enOption);

private:
	std::map<int, EquipItem*> m_mEquipItem;
	std::map<int, BundleItem*> m_mBundleItem;
	std::map<int, UpgradeItem*> m_mUpgradeItem;
	std::map<int, StateChangeItem*> m_mStateChangeItem;
	std::map<int, PortalScrollItem*> m_mPortalScrollItem;
	std::map<int, MobSummonItem*> m_mMobSummonItem;
	std::map<int, PetFoodItem*> m_mPetFoodItem;
	std::map<int, TamingMobFoodItem*> m_mTamingMobFoodItem;
	std::map<int, BridleItem*> m_mBridleItem;
	std::map<int, SkillLearnItem*> m_mSkillLearnItem;
	std::map<int, PortableChairItem*> m_mPortableChairItem;

	std::map<int, std::string> m_mItemString, m_mMapString;
	
	void LoadIncrementStat(BasicIncrementStat& refStat, void *pProp);
	void LoadAbilityStat(BasicAbilityStat& refStat, void *pProp);


	/*
	用於初始化物品的能力數值，其中v是預設數值，根據enOption來決定數值的偏差結果
	*/
	int GetVariation(int v, ItemVariationOption enOption);
};

