#include "ItemInfo.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotPet.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

ItemInfo::ItemInfo()
{
}


ItemInfo::~ItemInfo()
{
}

ItemInfo * ItemInfo::GetInstance()
{
	static ItemInfo* pInstance = new ItemInfo;
	return pInstance;
}

void ItemInfo::Initialize()
{
	IterateMapString(nullptr);
	LoadItemSellPriceByLv();
	WvsLogger::LogRaw("[ItemInfo::Initialize]開始載入所有物品名稱[IterateItemString Start]....\n");
	IterateItemString(nullptr);
	WvsLogger::LogRaw("[ItemInfo::Initialize]物品名稱載入完成[IterateItemString Done]....\n");

	static auto& eqpWz = stWzResMan->GetWz(Wz::Character);
	WvsLogger::LogRaw("[ItemInfo::Initialize]開始載入所有裝備[IterateEquipItem Start]....\n");
	IterateEquipItem(&eqpWz);
	WvsLogger::LogRaw("[ItemInfo::Initialize]裝備載入完成[IterateEquipItem Done]....\n");

	WvsLogger::LogRaw("[ItemInfo::Initialize]開始載入所有物品[IterateBundleItem Start]....\n");
	IterateBundleItem();
	WvsLogger::LogRaw("[ItemInfo::Initialize]物品載入完成[IterateBundleItem Done]....\n");
	//IterateCashItem();
	IteratePetItem();
	RegisterSpecificItems();
	RegisterNoRollbackItem();
	RegisterSetHalloweenItem();
	stWzResMan->ReleaseMemory();
	WvsLogger::LogRaw("[ItemInfo::Initialize]釋放ItemInfo所有Wz記憶體[ReleaseMemory Done]....\n");
}

void ItemInfo::LoadItemSellPriceByLv()
{
	auto& info = stWzResMan->GetWz(Wz::Item)["ItemSellPriceStandard"]["400"];
	for (auto& lvl : info)
		m_mItemSellPriceByLv[atoi(lvl.Name().c_str())] = (int)lvl;
}

void ItemInfo::IterateMapString(void *dataNode)
{
	static WZ::Node Img[] = {
		stWzResMan->GetWz(Wz::String)["Map"]
	};
	if (dataNode == nullptr)
		for (auto& img : Img)
			IterateMapString((void*)&img);
	else
	{
		auto& dataImg = (*((WZ::Node*)dataNode));
		for (auto& img : dataImg)
		{
			if (!isdigit(img.Name()[0]) || atoi(img.Name().c_str()) < 1000)
				IterateMapString((void*)&img);
			else
				m_mMapString[atoi(img.Name().c_str())] = img["mapName"];
		}
	}
}

void ItemInfo::IterateItemString(void *dataNode)
{
	static WZ::Node Img[] = { 
		stWzResMan->GetWz(Wz::String)["Eqp"], 
		stWzResMan->GetWz(Wz::String)["Etc"], 
		stWzResMan->GetWz(Wz::String)["Consume"], 
		stWzResMan->GetWz(Wz::String)["Ins"],
		stWzResMan->GetWz(Wz::String)["Cash"],
		stWzResMan->GetWz(Wz::String)["Pet"]
	};
	if (dataNode == nullptr)
		for (auto& img : Img)
			IterateItemString((void*)&img);
	else
	{
		auto& dataImg = (*((WZ::Node*)dataNode));
		for (auto& img : dataImg)
		{
			if (!isdigit(img.Name()[0]) || atoi(img.Name().c_str()) < 1000)
				IterateItemString((void*)&img);
			else
				m_mItemString[atoi(img.Name().c_str())] = img["name"];
		}
	}
}

void ItemInfo::IterateEquipItem(void *dataNode)
{
	auto& dataImg = (*((WZ::Node*)dataNode));
	int nItemID = 0;
	
	for (auto& data : dataImg)
	{
		if (data.Name() == "Hair" || data.Name() == "Face" || data.Name() == "Afterimage")
			continue;
		if (!isdigit(data.Name()[0])) //展開資料夾
		{
			clock_t tStart = clock();
			IterateEquipItem((void*)(&data));
			//printf("%s loading : %.2fs\n", data.Name().c_str(), (double)(clock() - tStart) / CLOCKS_PER_SEC);
		}
		else
		{
			nItemID = atoi(data.Name().c_str());
			if (nItemID < 20000)
				continue;
			EquipItem* pNewEquip = AllocObj(EquipItem);
			pNewEquip->nItemID = nItemID;
			pNewEquip->sItemName = m_mItemString[nItemID];
			RegisterEquipItemInfo(pNewEquip, nItemID, (void*)&(data));
			m_mEquipItem[nItemID] = pNewEquip;
		}
	}
}

void ItemInfo::IterateBundleItem()
{
	static WZ::Node Img[] = { stWzResMan->GetWz(Wz::Item)["Cash"]
		, stWzResMan->GetWz(Wz::Item)["Consume"]
		, stWzResMan->GetWz(Wz::Item)["Etc"]
		, stWzResMan->GetWz(Wz::Item)["Install"] };
	for (auto& baseImg : Img)
	{
		for (auto& dir : baseImg)
		{
			for (auto& item : dir)
			{
				auto& infoImg = item["info"];
				int nItemID = atoi(item.Name().c_str());
				BundleItem* pNewBundle = AllocObj( BundleItem );
				LoadAbilityStat(pNewBundle->abilityStat, (void*)&infoImg);
				if (pNewBundle->abilityStat.bCash)
					m_mCashItem.insert({ nItemID, AllocObj(CashItem) });

				pNewBundle->nItemID = nItemID;
				pNewBundle->sItemName = m_mItemString[nItemID];
				pNewBundle->nMaxPerSlot = infoImg["slotMax"];
				pNewBundle->dSellUnitPrice = (double)infoImg["unitPrice"];
				pNewBundle->nSellPrice = infoImg["price"];
				pNewBundle->nRequiredLEV = infoImg["reqLevel"];
				pNewBundle->nLevel = infoImg["lv"];
				if (pNewBundle->nSellPrice == 0)
					pNewBundle->nSellPrice = m_mItemSellPriceByLv[pNewBundle->nLevel];

				pNewBundle->nPAD = infoImg["incPAD"]; //飛鏢
				m_mBundleItem[nItemID] = pNewBundle;
				int nItemCategory = nItemID / 10000;
				void* pProp = (void*)&item;
				switch (nItemCategory)
				{
				case 200:
				case 201:
				case 202:
				case 205:
				case 221:
					RegisterStateChangeItem(nItemID, pProp);
					break;
				case 204:
					RegisterUpgradeItem(nItemID, pProp);
					break;
				case 203:
					RegisterPortalScrollItem(nItemID, pProp);
					break;
				case 210:
					RegisterMobSummonItem(nItemID, pProp);
					break;
				case 212:
					RegisterPetFoodItem(nItemID, pProp);
					break;
				case 226:
					RegisterTamingMobFoodItem(nItemID, pProp);
					break;
				case 227:
					RegisterBridleItem(nItemID, pProp);
					break;
				case 228:
				case 229:
					RegisterSkillLearnItem(nItemID, pProp);
					break;
				case 301:
					RegisterPortableChairItem(nItemID, pProp);
					break;
				}
			}
		}
	}
}

void ItemInfo::IteratePetItem()
{
	auto& img = stWzResMan->GetWz(Wz::Item)["Pet"];
	for (auto& item : img)
	{
		int nItemID = atoi(item.Name().c_str());
		CashItem *pItem = AllocObj(CashItem);
		pItem->bIsPet = true;
		m_mCashItem.insert({ nItemID, pItem });
	}
	
}

void ItemInfo::IterateCashItem()
{
	auto& img = stWzResMan->GetWz(Wz::Item)["Cash"];
	for (auto& subImg : img)
	{
		for (auto& item : subImg)
		{
			int nItemID = atoi(item.Name().c_str());
			CashItem *pItem = AllocObj(CashItem);
			m_mCashItem.insert({ nItemID, pItem });
		}
	}
}

void ItemInfo::RegisterSpecificItems()
{
}

void ItemInfo::RegisterNoRollbackItem()
{
}

void ItemInfo::RegisterSetHalloweenItem()
{
}

void ItemInfo::RegisterEquipItemInfo(EquipItem * pEqpItem, int nItemID, void * pProp)
{
	auto& infoImg = (*((WZ::Node*)pProp))["info"];

	LoadIncrementStat(pEqpItem->incStat, (void*)&infoImg);
	LoadAbilityStat(pEqpItem->abilityStat, (void*)&infoImg);

	if (pEqpItem->abilityStat.bCash)
		m_mCashItem.insert({ nItemID, AllocObj(CashItem) });

	pEqpItem->nItemID = nItemID;
	pEqpItem->nrSTR = infoImg["reqSTR"];
	pEqpItem->nrDEX = infoImg["reqDEX"];
	pEqpItem->nrINT = infoImg["reqINT"];
	pEqpItem->nrLUK = infoImg["reqLUK"];
	pEqpItem->nrPOP = infoImg["reqPOP"];
	pEqpItem->nrJob = infoImg["reqJob"];
	pEqpItem->nrLevel = infoImg["reqLevel"];
	pEqpItem->nRUC = infoImg["tuc"];
	//pEqpItem->nrMobLevel = infoImg["reqMobLevel"];
	pEqpItem->nSellPrice = infoImg["price"];
	pEqpItem->nKnockBack = infoImg["knockback"];
	pEqpItem->nIncRMAF = infoImg["incRMAF"];
	pEqpItem->nIncRMAI = infoImg["incRMAI"];
	pEqpItem->nIncRMAL = infoImg["incRMAL"];
	pEqpItem->nElemDefault = infoImg["elemDefault"];
	pEqpItem->nCuttable = (int)infoImg["tradeAvailable"];
	pEqpItem->dwPetAbilityFlag = 0;
	if (nItemID / 10000 == 181)
	{
		if ((int)infoImg["pickupItem"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x1;

		if ((int)infoImg["longRange"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x2;

		if ((int)infoImg["pickupOthers"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x10;

		if ((int)infoImg["consumeHP"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x20;

		if ((int)infoImg["consumeMP"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x40;
	}
}

void ItemInfo::RegisterUpgradeItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WZ::Node*)pProp))["info"];
	UpgradeItem *pNewUpgradeItem = AllocObj(UpgradeItem);

	pNewUpgradeItem->nItemID = nItemID;
	LoadIncrementStat(pNewUpgradeItem->incStat, (void*)&infoImg);
	pNewUpgradeItem->nSuccessRate = infoImg["success"];
	pNewUpgradeItem->nCursedRate = infoImg["cursed"];
	m_mUpgradeItem[nItemID] = pNewUpgradeItem;
}

void ItemInfo::RegisterPortalScrollItem(int nItemID, void * pProp)
{
	PortalScrollItem *pNewPortalScrollItem = AllocObj(PortalScrollItem);
	pNewPortalScrollItem->nItemID = nItemID;
	auto& specImg = (*((WZ::Node*)pProp))["spec"];
	for (auto& effect : specImg)
		pNewPortalScrollItem->spec.insert({ effect.Name(), (int)effect });
	m_mPortalScrollItem[nItemID] = pNewPortalScrollItem;
}

void ItemInfo::RegisterMobSummonItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WZ::Node*)pProp))["info"];
	MobSummonItem *pNewMobSummonItem = AllocObj(MobSummonItem);
	pNewMobSummonItem->nItemID = nItemID;
	pNewMobSummonItem->nType = infoImg["type"];
	auto& mobImg = (*((WZ::Node*)pProp))["mob"];
	for (auto& mob : mobImg)
		pNewMobSummonItem->lMob.insert({ (int)(mob["id"]), (int)(mob["prob"]) });
	m_mMobSummonItem[nItemID] = pNewMobSummonItem;
}

void ItemInfo::RegisterPetFoodItem(int nItemID, void * pProp)
{
	auto& specImg = (*((WZ::Node*)pProp))["spec"];
	PetFoodItem *pNewFoodItem = AllocObj(PetFoodItem);
	pNewFoodItem->nItemID = nItemID;
	pNewFoodItem->niRepleteness = specImg["inc"];
	for (auto& petID : specImg)
		if (isdigit(petID.Name()[0]))
			pNewFoodItem->ldwPet.push_back((int)petID);
	m_mPetFoodItem[nItemID] = pNewFoodItem;
}

void ItemInfo::RegisterTamingMobFoodItem(int nItemID, void * pProp)
{
	auto& specImg = (*((WZ::Node*)pProp))["spec"];
	TamingMobFoodItem *pNewTamingMobFoodItem = AllocObj(TamingMobFoodItem);
	pNewTamingMobFoodItem->nItemID = nItemID;
	pNewTamingMobFoodItem->niFatigue = specImg["incFatigue"];
	m_mTamingMobFoodItem[nItemID] = pNewTamingMobFoodItem;
}

void ItemInfo::RegisterBridleItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WZ::Node*)pProp))["info"];
	BridleItem *pNewBridleItem = AllocObj(BridleItem);
	pNewBridleItem->nItemID = nItemID;
	pNewBridleItem->dwTargetMobID = infoImg["mob"];
	pNewBridleItem->nCreateItemID = infoImg["create"];
	pNewBridleItem->nBridleProp = infoImg["bridleProp"];
	pNewBridleItem->nBridleMsgType = infoImg["bridleMsgType"];
	pNewBridleItem->nBridleHP = infoImg["mobHP"];
	pNewBridleItem->nUseDelay = infoImg["useDelay"];
	pNewBridleItem->dBridlePropChg = (double)infoImg["bridlePropChg"];
	m_mBridleItem[nItemID] = pNewBridleItem;
}

void ItemInfo::RegisterPortableChairItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WZ::Node*)pProp))["info"];
	PortableChairItem *pNewPortableChairItem = AllocObj(PortableChairItem);
	pNewPortableChairItem->nItemID = nItemID;
	pNewPortableChairItem->nReqLevel = infoImg["reqLevel"];
	pNewPortableChairItem->nPortableChairRecoveryRateMP = infoImg["recoveryMP"];
	pNewPortableChairItem->nPortableChairRecoveryRateHP = infoImg["recoveryHP"];
	m_mPortableChairItem[nItemID] = pNewPortableChairItem;
}

void ItemInfo::RegisterSkillLearnItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WZ::Node*)pProp))["info"];
	SkillLearnItem *pNewSkillLearnItem = AllocObj(SkillLearnItem);
	pNewSkillLearnItem->nItemID = nItemID;
	pNewSkillLearnItem->nMasterLevel = infoImg["masterLevel"];
	pNewSkillLearnItem->nSuccessRate = infoImg["success"];
	if (nItemID / 10000 == 229)
		pNewSkillLearnItem->nReqLevel = infoImg["reqSkillLevel"];
	auto& skillImg = infoImg["skill"];
	for (auto& skill : skillImg)
		pNewSkillLearnItem->aSkill.push_back((int)skill);
	m_mSkillLearnItem[nItemID] = pNewSkillLearnItem;
}

void ItemInfo::RegisterStateChangeItem(int nItemID, void * pProp)
{
	StateChangeItem *pNewStateChangeItem = AllocObj(StateChangeItem);
	pNewStateChangeItem->nItemID = nItemID;
	auto& specImg = (*((WZ::Node*)pProp))["spec"];
	for(auto& effect : specImg)
		pNewStateChangeItem->spec.insert({ effect.Name(), (int)effect });
	m_mStateChangeItem[nItemID] = pNewStateChangeItem;
}

EquipItem * ItemInfo::GetEquipItem(int nItemID)
{
	auto findIter = m_mEquipItem.find(nItemID);
	return (findIter != m_mEquipItem.end() ? findIter->second : nullptr);
}

StateChangeItem * ItemInfo::GetStateChangeItem(int nItemID)
{
	auto findIter = m_mStateChangeItem.find(nItemID);
	return (findIter != m_mStateChangeItem.end() ? findIter->second : nullptr);
}

CashItem * ItemInfo::GetCashItem(int nItemID)
{
	auto findIter = m_mCashItem.find(nItemID);
	return (findIter != m_mCashItem.end() ? findIter->second : nullptr);
}

BundleItem * ItemInfo::GetBundleItem(int nItemID)
{
	auto findIter = m_mBundleItem.find(nItemID);
	return (findIter != m_mBundleItem.end() ? findIter->second : nullptr);
}

UpgradeItem * ItemInfo::GetUpgradeItem(int nItemID)
{
	auto findIter = m_mUpgradeItem.find(nItemID);
	return (findIter != m_mUpgradeItem.end() ? findIter->second : nullptr);
}

PortalScrollItem * ItemInfo::GetPortalScrollItem(int nItemID)
{
	auto findIter = m_mPortalScrollItem.find(nItemID);
	return (findIter != m_mPortalScrollItem.end() ? findIter->second : nullptr);
}

MobSummonItem * ItemInfo::GetMobSummonItem(int nItemID)
{
	auto findIter = m_mMobSummonItem.find(nItemID);
	return (findIter != m_mMobSummonItem.end() ? findIter->second : nullptr);
}

PetFoodItem * ItemInfo::GetPetFoodItem(int nItemID)
{
	auto findIter = m_mPetFoodItem.find(nItemID);
	return (findIter != m_mPetFoodItem.end() ? findIter->second : nullptr);
}

TamingMobFoodItem * ItemInfo::GetTamingMobFoodItem(int nItemID)
{
	auto findIter = m_mTamingMobFoodItem.find(nItemID);
	return (findIter != m_mTamingMobFoodItem.end() ? findIter->second : nullptr);
}

BridleItem * ItemInfo::GetBridleItem(int nItemID)
{
	auto findIter = m_mBridleItem.find(nItemID);
	return (findIter != m_mBridleItem.end() ? findIter->second : nullptr);
}

SkillLearnItem * ItemInfo::GetSkillLearnItem(int nItemID)
{
	auto findIter = m_mSkillLearnItem.find(nItemID);
	return (findIter != m_mSkillLearnItem.end() ? findIter->second : nullptr);
}

PortableChairItem * ItemInfo::GetPortableChairItem(int nItemID)
{
	auto findIter = m_mPortableChairItem.find(nItemID);
	return (findIter != m_mPortableChairItem.end() ? findIter->second : nullptr);
}

int ItemInfo::GetItemSlotType(int nItemID)
{
	return nItemID / 1000000;
}

bool ItemInfo::IsTreatSingly(int nItemID, long long int liExpireDate)
{
	int nItemHeader = GetItemSlotType(nItemID);
	return ((nItemHeader != 2 && nItemHeader != 3 && nItemHeader != 4)
		|| IsRechargable(nItemID)
		/*|| liExpireDate != 0*/);
}

bool ItemInfo::IsRechargable(int nItemID)
{
	return nItemID / 10000 == 207 || nItemID / 10000 == 233;
}

bool ItemInfo::ConsumeOnPickup(int nItemID)
{
	StateChangeItem* pItem = nullptr;
	if (nItemID / 1000000 == 2 && (pItem = GetStateChangeItem(nItemID)) != nullptr)
	{
		for (auto& p : pItem->spec)
			if (p.first == "consumeOnPickup" && p.second == 1)
				return true;
	}
	return false;
}

bool ItemInfo::ExpireOnLogout(int nItemID)
{
	if (nItemID / 1000000 != 5)
		if (nItemID / 1000000 == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eExpireOnLogout) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eExpireOnLogout) != 0;
		}
	return false;
}

int ItemInfo::GetBulletPAD(int nItemID)
{
	auto pItem = GetBundleItem(nItemID);
	if (pItem == nullptr)
		return 0;
	return pItem->nPAD;
}

long long int ItemInfo::GetItemDateExpire(const std::string & sDate)
{
	std::string sYear = sDate.substr(4);
	std::string sMonth = sDate.substr(4, 2);
	std::string sDay = sDate.substr(6, 2);
	std::string sHour = sDate.substr(8, 2);
	SYSTEMTIME sysTime;
	sysTime.wYear = atoi(sYear.c_str());
	sysTime.wMonth = atoi(sMonth.c_str());
	sysTime.wDay = atoi(sDay.c_str());
	sysTime.wHour = atoi(sHour.c_str());
	sysTime.wMilliseconds = 0;
	sysTime.wSecond = 0;
	sysTime.wMinute = 0;
	sysTime.wDayOfWeek = 0;
	FILETIME ft;
	SystemTimeToFileTime(&sysTime, &ft);
	return *((long long int*)&ft);
}

const std::string & ItemInfo::GetItemName(int nItemID)
{
	static std::string strEmpty = "";
	auto findResult = m_mItemString.find(nItemID);
	return (findResult == m_mItemString.end() ? strEmpty : findResult->second);
}

bool ItemInfo::IsAbleToEquip(int nGender, int nLevel, int nJob, int nSTR, int nDEX, int nINT, int nLUK, int nPOP, GW_ItemSlotBase * pPetItem, int nItemID)
{
	return false;
}

bool ItemInfo::IsNotSaleItem(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	if (nTI != 5)
		if (nTI == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eNotSale) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eNotSale) != 0;
		}
	return false;
}

bool ItemInfo::IsOnlyItem(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	if (nTI != 5)
		if (nTI == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eOnly) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eOnly) != 0;
		}
	return false;
}

bool ItemInfo::IsTradeBlockItem(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	if (nTI != 5)
		if (nTI == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eTradeBlock) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eTradeBlock) != 0;
		}
	return false;
}

bool ItemInfo::IsQuestItem(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	if (nTI != 5)
		if (nTI == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eQuest) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eQuest) != 0;
		}
	return false;
}

bool ItemInfo::IsWeapon(int nItemID)
{
	return (nItemID >= 1210000 && nItemID < 1600000)
		|| nItemID / 10000 == 135;
}

bool ItemInfo::IsCoat(int nItemID)
{
	return nItemID / 10000 == 104;
}

bool ItemInfo::IsCape(int nItemID)
{
	return nItemID / 10000 == 100;
}

bool ItemInfo::IsPants(int nItemID)
{
	return nItemID / 10000 == 106;
}

bool ItemInfo::IsHair(int nItemID)
{
	return nItemID >= 30000 && nItemID <= 49999;
}

bool ItemInfo::IsFace(int nItemID)
{
	return nItemID >= 20000 && nItemID <= 29999;
}

bool ItemInfo::IsShield(int nItemID)
{
	return nItemID / 10000 == 109;
}

bool ItemInfo::IsShoes(int nItemID)
{
	return nItemID / 10000 == 107;
}

bool ItemInfo::IsLongcoat(int nItemID)
{
	return nItemID / 10000 == 105;
}

bool ItemInfo::IsCap(int nItemID)
{
	return nItemID / 10000 == 110;
}

bool ItemInfo::IsPet(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	int nPrefix = nItemID / 1000;
	return nTI == GW_ItemSlotBase::CASH && (nPrefix % 10 == 0);
}

void ItemInfo::LoadIncrementStat(BasicIncrementStat & refStat, void * pProp)
{
	auto& infoImg = (*((WZ::Node*)pProp));
	refStat.niSTR = infoImg["incSTR"];
	refStat.niDEX = infoImg["incDEX"];
	refStat.niINT = infoImg["incINT"];
	refStat.niLUK = infoImg["incLUK"];
	refStat.niMaxHP = infoImg["incMHP"];
	refStat.niMaxMP = infoImg["incMMP"];
	refStat.niPAD = infoImg["incPAD"];
	refStat.niMAD = infoImg["incMAD"];
	refStat.niPDD = infoImg["incPDD"];
	refStat.niMDD = infoImg["incMDD"];
	refStat.niACC = infoImg["incACC"];
	refStat.niEVA = infoImg["incEVA"];
	refStat.niCraft = infoImg["incCraft"];
	refStat.niSpeed = infoImg["incSpeed"];
	refStat.niJump = infoImg["incJump"];
	refStat.niSwim = infoImg["incSwim"];
}

void ItemInfo::LoadAbilityStat(BasicAbilityStat & refStat, void * pProp)
{
	auto& infoImg = (*((WZ::Node*)pProp));	
	refStat.nAttribute = (int)infoImg["bagType"];
	if (((int)infoImg["notSale"]) == 1)
		refStat.nAttribute |= ItemAttribute::eNotSale;
	if (((int)infoImg["expireOnLogout"]) == 1)
		refStat.nAttribute |= ItemAttribute::eExpireOnLogout;
	if (((int)infoImg["pickUpBlock"] == 1))
		refStat.nAttribute |= ItemAttribute::ePickUpBlock;
	if (((int)infoImg["equipTradeBlock"] == 1))
		refStat.nAttribute |= ItemAttribute::eTradeBlockAfterEquip;
	if (((int)infoImg["only"]) == 1)
		refStat.nAttribute |= ItemAttribute::eOnly;
	if (((int)infoImg["accountSharable"] == 1))
		refStat.nAttribute |= ItemAttribute::eAccountSharable;
	if (((int)infoImg["quest"]) == 1)
		refStat.nAttribute |= ItemAttribute::eQuest;
	if (((int)infoImg["tradeBlock"]) == 1)
		refStat.nAttribute |= ItemAttribute::eTradeBlock;
	if (((int)infoImg["accountShareTag"] == 1))
		refStat.nAttribute |= ItemAttribute::eAccountShareTag;
	if (((int)infoImg["mobHP"] == 1) && (int)infoImg["mobHP"] < 100)
		refStat.nAttribute |= ItemAttribute::eMobHP;

	refStat.bCash = ((int)infoImg["cash"]) == 1;
	refStat.bTimeLimited = ((int)infoImg["timeLimited"]) == 1;
}

GW_ItemSlotBase * ItemInfo::GetItemSlot(int nItemID, ItemVariationOption enOption)
{
	int nType = GetItemSlotType(nItemID);
	GW_ItemSlotBase *ret = nullptr;
	if (nType == 1)
	{
		auto pItem = GetEquipItem(nItemID);
		if (pItem == nullptr)
			return nullptr;
		GW_ItemSlotEquip* pEquip = AllocObj(GW_ItemSlotEquip);
		int nValue = 0;
		if ((nValue = pItem->incStat.niSTR))
			pEquip->nSTR = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niDEX))
			pEquip->nDEX = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niLUK))
			pEquip->nLUK = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niINT))
			pEquip->nINT = GetVariation(nValue, enOption);

		if ((nValue = pItem->incStat.niMaxHP))
			pEquip->nMaxHP = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niMaxMP))
			pEquip->nMaxMP = GetVariation(nValue, enOption);

		if ((nValue = pItem->incStat.niPAD))
			pEquip->nPAD = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niPDD))
			pEquip->nPDD = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niMAD))
			pEquip->nMAD = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niMDD))
			pEquip->nMDD = GetVariation(nValue, enOption);

		if ((nValue = pItem->incStat.niACC))
			pEquip->nACC = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niEVA))
			pEquip->nEVA = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niCraft))
			pEquip->nCraft = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niSpeed))
			pEquip->nSpeed = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niJump))
			pEquip->nJump = GetVariation(nValue, enOption);
		pEquip->nRUC = pItem->nRUC;
		pEquip->nCuttable = (pItem->nCuttable == 0 ? 0xFF : pItem->nCuttable);

		ret = pEquip;
		ret->nAttribute = pItem->abilityStat.nAttribute;
		ret->bIsCash = pItem->abilityStat.bCash;
	}
	else if (nType > 1 && nType <= 5)
	{
		auto pItem = GetBundleItem(nItemID);
		if (pItem == nullptr)
		{
			auto pCash = GetCashItem(nItemID);
			if (pCash && pCash->bIsPet)
			{
				ret = AllocObj(GW_ItemSlotPet);
				((GW_ItemSlotPet*)ret)->nLevel = 1;
				((GW_ItemSlotPet*)ret)->strPetName = GetItemName(nItemID);
				ret->bIsCash = ret->bIsPet = true;
			}
		}
		else
		{
			ret = AllocObj(GW_ItemSlotBundle);
			((GW_ItemSlotBundle*)ret)->nNumber = 1;
			ret->nAttribute = pItem->abilityStat.nAttribute;
			ret->bIsCash = pItem->abilityStat.bCash;
		}
	}
	if (ret)
	{
		ret->nItemID = nItemID;
		ret->nType = (GW_ItemSlotBundle::GW_ItemSlotType)((nType));
	}
	return ret;
}

int ItemInfo::GetVariation(int v, ItemVariationOption enOption)
{
	int result; // eax@1
	int v3; // eax@3
	unsigned int v4; // edi@5
	signed int v5; // eax@6
	int v6; // ecx@8
	signed int v7; // edi@8
	int v8; // edi@11
	int v9; // eax@16
	unsigned int v10; // edi@18
	signed int v11; // eax@19
	char v12; // cl@21
	signed int v13; // eax@21
	int v14; // edi@21
	bool v15; // zf@24
	bool v16; // cf@28
	int enOptiona; // [sp+18h] [bp+Ch]@5

	result = v;
	if (v)
	{
		if (enOption != 4)
		{
			v9 = v / 10 + 1;
			if (v9 >= 5)
				v9 = 5;
			v10 = 1 << (v9 + 2);
			if (v10)
				v11 = Rand32::GetInstance()->Random() % v10;
			else
				v11 = Rand32::GetInstance()->Random();
			v12 = v11;
			v13 = v11 >> 1;
			v14 = ((v13 >> 3) & 1)
				+ ((v13 >> 2) & 1)
				+ ((v13 >> 1) & 1)
				+ (v13 & 1)
				+ (v12 & 1)
				- 2
				+ ((v13 >> 4) & 1)
				+ ((v13 >> 5) & 1);
			if (v14 <= 0)
				v14 = 0;
			if (enOption == 2)
			{
				v15 = (Rand32::GetInstance()->Random() & 1) == 0;
				result = v;
				if (v15)
					return v - v14;
			}
			else
			{
				if (enOption == 1)
					v16 = Rand32::GetInstance()->Random() % 0xA < 3;
				else
				{
					if (enOption != 3)
						return v;
					v16 = Rand32::GetInstance()->Random() % 0xA < 1;
				}
				result = v;
				if (v16)
					return result;
			}
			result += v14;
			return result;
		}
		v3 = v / 5 + 1;
		if (v3 >= 7)
			v3 = 7;
		v4 = 1 << (v3 + 2);
		enOptiona = v3 + 2;
		if (v4)
			v5 = Rand32::GetInstance()->Random() % v4;
		else
			v5 = Rand32::GetInstance()->Random();
		v6 = enOptiona;
		v7 = -2;
		if ((signed int)enOptiona > 0)
		{
			do
			{
				v7 += v5 & 1;
				v5 >>= 1;
				--v6;
			} while (v6);
		}
		if (Rand32::GetInstance()->Random() & 1)
			v8 = v + v7;
		else
			v8 = v - v7;
		if (v8 <= 0)
			v8 = 0;
		result = v8;
	}
	return result;
}

