#include "CharacterDBAccessor.h"
#include "GW_ItemSlotEquip.h"
#include "GW_ItemSlotBundle.h"
#include "GW_ItemSlotPet.h"
#include "GW_CharacterStat.h"
#include "GW_CharacterLevel.h"
#include "GW_Avatar.hpp"
#include "GW_Account.h"
#include "GW_CashItemInfo.h"
#include "GW_CharacterSlotCount.h"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\ShopPacketFlags.hpp"

#include "GW_CharacterList.hpp"
#include "GA_Character.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "GW_FuncKeyMapped.h"

//WvsUnified CharacterDBAccessor::mDBUnified;

CharacterDBAccessor::CharacterDBAccessor()
{
}


CharacterDBAccessor::~CharacterDBAccessor()
{
}

void CharacterDBAccessor::PostLoadCharacterListRequest(SocketBase *pSrv, int uLocalSocketSN, int nAccountID, int nWorldID)
{
	OutPacket oPacket;
	GW_CharacterList chrList;
	chrList.Load(nAccountID, nWorldID);
	oPacket.Encode2(CenterSendPacketFlag::CharacterListResponse);
	oPacket.Encode4(uLocalSocketSN);
	oPacket.Encode4(chrList.nCount);
	for (int i = 0; i < chrList.nCount; ++i)
		oPacket.Encode4(chrList.aCharacterList[i]);
	oPacket.Encode1(chrList.nCount);
	for (int i = 0; i < chrList.nCount; ++i)
	{
		GA_Character chrEntry;
		chrEntry.LoadAvatar(chrList.aCharacterList[i]);
		chrEntry.EncodeAvatar(&oPacket);
		oPacket.Encode1(0);
		oPacket.Encode1(0); // Ranking
	}

	pSrv->SendPacket(&oPacket);
}

void CharacterDBAccessor::PostCreateNewCharacterRequest(SocketBase *pSrv, int uLocalSocketSN, int nAccountID, int nWorldID, const std::string& strName, int nGender, int nFace, int nHair, int nSkin, const int* aBody, const int* aStat)
{
	GA_Character chrEntry;
	chrEntry.nAccountID = nAccountID;
	chrEntry.nWorldID = nWorldID;
	chrEntry.strName = strName;
	chrEntry.mStat->nGender = nGender;

	chrEntry.nFieldID = 100000000;
	chrEntry.nGuildID = chrEntry.nPartyID = chrEntry.mStat->nFame = 0;
	
	chrEntry.mStat->nFace = nFace;
	chrEntry.mStat->nHair = nHair;
	chrEntry.mStat->nSkin = nSkin;

	chrEntry.mStat->nStr = aStat[STAT_Str];
	chrEntry.mStat->nDex = aStat[STAT_Dex];
	chrEntry.mStat->nInt = aStat[STAT_Int];
	chrEntry.mStat->nLuk = aStat[STAT_Luk];
	chrEntry.mStat->nHP = aStat[STAT_HP];
	chrEntry.mStat->nMP = aStat[STAT_MP];
	chrEntry.mStat->nMaxHP = aStat[STAT_MaxHP];
	chrEntry.mStat->nMaxMP = aStat[STAT_MaxMP];
	chrEntry.mStat->nJob = aStat[STAT_Job];
	chrEntry.mStat->nSubJob = aStat[STAT_SubJob];
	chrEntry.mLevel->nLevel = aStat[STAT_Level];
	chrEntry.mStat->nAP = aStat[STAT_AP];

	chrEntry.mSlotCount->aSlotCount[GW_ItemSlotBase::EQUIP] = 40;
	chrEntry.mSlotCount->aSlotCount[GW_ItemSlotBase::CONSUME] = 40;
	chrEntry.mSlotCount->aSlotCount[GW_ItemSlotBase::ETC] = 40;
	chrEntry.mSlotCount->aSlotCount[GW_ItemSlotBase::INSTALL] = 40;
	chrEntry.mSlotCount->aSlotCount[GW_ItemSlotBase::CASH] = 40;

	GW_ItemSlotEquip gwCapEquip;
	gwCapEquip.nItemID = aBody[EQP_ID_CapEquip];
	gwCapEquip.nPOS = EQP_POS_Cap;

	GW_ItemSlotEquip gwCoatEquip;
	gwCoatEquip.nItemID = aBody[EQP_ID_CoatEquip];
	gwCoatEquip.nPOS = EQP_POS_Coat;

	GW_ItemSlotEquip gwPantsEquip;
	gwPantsEquip.nItemID = aBody[EQP_ID_PantsEquip];
	gwPantsEquip.nPOS = EQP_POS_Pants;

	GW_ItemSlotEquip gwWeaponEquip;
	gwWeaponEquip.nItemID = aBody[EQP_ID_WeaponEquip];
	gwWeaponEquip.nPOS = EQP_POS_Weapon;

	GW_ItemSlotEquip gwShoesEquip;
	gwShoesEquip.nItemID = aBody[EQP_ID_ShoesEquip];
	gwShoesEquip.nPOS = EQP_POS_Shoes;

	GW_ItemSlotEquip gwCapeEquip;
	gwCapeEquip.nItemID = aBody[EQP_ID_CapeEquip];
	gwCapeEquip.nPOS = EQP_POS_Cape;

	GW_ItemSlotEquip gwShieldEquip;
	gwShieldEquip.nItemID = aBody[EQP_ID_ShieldEquip];
	gwShieldEquip.nPOS = EQP_POS_Shield;

	GW_ItemSlotEquip* equips[EQP_ID_FLAG_END] = {
		&gwCapEquip,
		&gwCoatEquip,
		&gwPantsEquip,
		&gwWeaponEquip,
		&gwShoesEquip,
		&gwCapeEquip,
		&gwShieldEquip
	};
	int nEquipCount = sizeof(equips) / sizeof(GW_ItemSlotBase*);
	for (int i = 0; i < nEquipCount; ++i)
		if (equips[i]->nItemID > 0) 
		{
			equips[i]->nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
			chrEntry.mItemSlot[1].insert({ equips[i]->nPOS, equips[i] });
		}
	chrEntry.Save(true);

	//Since items here are auto-var, they will destruct automatically
	//Prevent GA_Character from deleting those items.
	for (int i = 0; i < nEquipCount; ++i)
		if (equips[i]->nItemID > 0)
			chrEntry.mItemSlot[1].erase(equips[i]->nPOS);
	GW_FuncKeyMapped funcKeyMapped(chrEntry.nCharacterID);
	funcKeyMapped.Save(true);
}

void CharacterDBAccessor::GetDefaultCharacterStat(int *aStat)
{
	aStat[STAT_Str] = 10;
	aStat[STAT_Dex] = 10;
	aStat[STAT_Int] = 10;
	aStat[STAT_Luk] = 10;
	aStat[STAT_HP] = 50;
	aStat[STAT_MP] = 50;
	aStat[STAT_MaxHP] = 50;
	aStat[STAT_MaxMP] = 50;
	aStat[STAT_Job] = 0;
	aStat[STAT_SubJob] = 0;
	aStat[STAT_Level] = 1;
	aStat[STAT_AP] = 0;
}

void CharacterDBAccessor::PostCharacterDataRequest(SocketBase *pSrv, int nClientSocketID, int nCharacterID, void *oPacket_)
{
	GA_Character chrEntry;
	chrEntry.Load(nCharacterID);
	OutPacket *oPacket = (OutPacket*)oPacket_;
	oPacket->Encode4(chrEntry.nAccountID);
	chrEntry.EncodeCharacterData(oPacket, true);
	GW_FuncKeyMapped funcKeyMapped(chrEntry.nCharacterID);
	funcKeyMapped.Load();
	funcKeyMapped.Encode(oPacket);
}

void CharacterDBAccessor::PostBuyCashItemRequest(SocketBase * pSrv, int uClientSocketSN, int nCharacterID, void * iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;

	int nAccountID = iPacket->Decode4();
	int nChargeType = iPacket->Decode1();
	int nType = iPacket->Decode1();
	bool bIsPet = iPacket->Decode1() == 1;
	int nPrice = iPacket->Decode4();
	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);
	GW_Account account;
	account.Load(nAccountID);

	if (account.QueryCash(nChargeType) >= nPrice)
	{
		oPacket.Encode2(ShopInternalPacketFlag::OnCenterResBuyDone);
		account.UpdateCash(nChargeType, -nPrice);

		GW_ItemSlotBase* pItem = nullptr;
		if (nType == GW_ItemSlotBase::GW_ItemSlotType::EQUIP)
			pItem = AllocObj(GW_ItemSlotEquip);
		else if (bIsPet)
			pItem = AllocObj(GW_ItemSlotPet);
		else
			pItem = AllocObj(GW_ItemSlotBundle);

		pItem->nType = (nType == GW_ItemSlotBase::GW_ItemSlotType::EQUIP ?
			  GW_ItemSlotBase::GW_ItemSlotType::EQUIP
			: GW_ItemSlotBase::GW_ItemSlotType::CASH);
		pItem->bIsCash = true;
		pItem->bIsPet = bIsPet;
		pItem->DecodeInventoryPosition(iPacket);
		pItem->Decode(iPacket, false);
		pItem->nCharacterID = nCharacterID;
		//pItem->liCashItemSN = GW_ItemSlotBase::IncItemSN(GW_ItemSlotBase::GW_ItemSlotType::CASH);
		pItem->nPOS = GW_ItemSlotBase::LOCK_POS;
		pItem->liItemSN = -1;
		pItem->Save(nCharacterID);

		GW_CashItemInfo cashItemInfo;
		cashItemInfo.Decode(iPacket);
		cashItemInfo.nAccountID = nAccountID;
		cashItemInfo.cashItemOption.liCashItemSN = pItem->liCashItemSN;
		cashItemInfo.bLocked = true;
		cashItemInfo.nGWItemSlotInstanceType = pItem->nInstanceType;
		cashItemInfo.Save(true);

		oPacket.Encode4(account.QueryCash(1));
		oPacket.Encode4(account.QueryCash(2));
		cashItemInfo.Encode(&oPacket);
	}
	else
		oPacket.Encode2(ShopInternalPacketFlag::OnCenterResBuyFailed);

	pSrv->SendPacket(&oPacket);
}


void CharacterDBAccessor::PostLoadLockerRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	int nAccountID = iPacket->Decode4();
	auto aRes = GW_CashItemInfo::LoadAll(nAccountID);
	std::vector<GW_ItemSlotPet> aPet;
	OutPacket oPacket;
	oPacket.Encode2((short)CenterSendPacketFlag::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode2(ShopInternalPacketFlag::OnCenterLoadLockerDone);
	oPacket.Encode2((short)aRes.size());
	decltype(aRes) aResWithoutPet;
	for (auto& info : aRes)
	{
		if (info.nGWItemSlotInstanceType != GW_ItemSlotBase::GW_ItemSlotInstanceType::GW_ItemSlotPet_Type)
			aResWithoutPet.push_back(info);
		info.Encode(&oPacket);
		if (info.nGWItemSlotInstanceType == GW_ItemSlotBase::GW_ItemSlotInstanceType::GW_ItemSlotPet_Type)
		{
			aPet.push_back({});
			aPet[(int)aPet.size() - 1].Load(info.cashItemOption.liCashItemSN);
		}
	}
	//oPacket.Encode2(aResWithoutPet.size());
	//for (auto& info : aResWithoutPet)
	//	info.Encode(&oPacket);
	oPacket.Encode4(0);
	//for (auto& pet : aPet)
	//	pet.Encode(&oPacket, false);

	oPacket.Encode2(0);
	oPacket.Encode2(0);
	oPacket.Encode2(0);
	oPacket.Encode2(0);

	pSrv->SendPacket(&oPacket);
}

void CharacterDBAccessor::PostUpdateCashRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	int nAccountID = iPacket->Decode4();
	GW_Account account;
	account.Load(nAccountID);

	OutPacket oPacket;
	oPacket.Encode2((short)CenterSendPacketFlag::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode2(ShopInternalPacketFlag::OnCenterUpdateCashDone);

	oPacket.Encode4(account.nNexonCash);
	oPacket.Encode4(account.nMaplePoint);
	oPacket.Encode4(0);
	oPacket.Encode4(0);

	pSrv->SendPacket(&oPacket);
}

void CharacterDBAccessor::PostMoveSlotToLockerRequest(SocketBase * pSrv, int uClientSocketSN, int nCharacterID, void * iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	int nAccountID = iPacket->Decode4();
	long long int liCashItemSN = iPacket->Decode8();
	int nType = iPacket->Decode1();
	GW_CashItemInfo cashItemInfo;
	cashItemInfo.Load(liCashItemSN);
	cashItemInfo.bLocked = true;
	cashItemInfo.Save();
	GW_ItemSlotBase *pItem = nullptr;

	if (cashItemInfo.nGWItemSlotInstanceType == GW_ItemSlotBase::GW_ItemSlotEquip_Type)
		pItem = AllocObj(GW_ItemSlotEquip);
	else if (cashItemInfo.nGWItemSlotInstanceType == GW_ItemSlotBase::GW_ItemSlotPet_Type)
		pItem = AllocObj(GW_ItemSlotPet);
	else
	{
		pItem = AllocObj(GW_ItemSlotBundle);
		pItem->nType = GW_ItemSlotBase::GW_ItemSlotType::CASH;
	}

	pItem->bIsCash = true;
	pItem->Load(liCashItemSN);
	pItem->nPOS = GW_ItemSlotBase::LOCK_POS;
	pItem->Save(nCharacterID);
	pItem->Release();

	OutPacket oPacket;
	oPacket.Encode2((short)CenterSendPacketFlag::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode2(ShopInternalPacketFlag::OnCenterMoveToLockerDone);
	oPacket.Encode8(liCashItemSN);
	oPacket.Encode1(nType);
	cashItemInfo.Encode(&oPacket);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	pSrv->SendPacket(&oPacket);
}

void CharacterDBAccessor::PostMoveLockerToSlotRequest(SocketBase * pSrv, int uClientSocketSN, int nCharacterID, void * iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	int nAccountID = iPacket->Decode4();
	long long int liCashItemSN = iPacket->Decode8();
	OutPacket oPacket;
	oPacket.Encode2((short)CenterSendPacketFlag::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);

	GW_CashItemInfo cashItemInfo;
	cashItemInfo.Load(liCashItemSN);
	GW_ItemSlotBase *pItem = nullptr;
	GA_Character characterData;
	characterData.Load(nCharacterID);

	if (cashItemInfo.nGWItemSlotInstanceType == GW_ItemSlotBase::GW_ItemSlotEquip_Type)
		pItem = AllocObj(GW_ItemSlotEquip);
	else if (cashItemInfo.nGWItemSlotInstanceType == GW_ItemSlotBase::GW_ItemSlotPet_Type)
		pItem = AllocObj(GW_ItemSlotPet);
	else 
	{
		pItem = AllocObj(GW_ItemSlotBundle);
		pItem->nType = GW_ItemSlotBase::GW_ItemSlotType::CASH;
	}

	pItem->bIsCash = true;
	pItem->Load(liCashItemSN);
	auto nPOS = characterData.FindEmptySlotPosition((int)pItem->nType);
	if (!nPOS)
	{
		oPacket.Encode2(ShopInternalPacketFlag::OnCenterMoveToSlotFailed);
		pSrv->SendPacket(&oPacket);
		return;
	}
	pItem->nPOS = nPOS;
	pItem->Save(nCharacterID);
	cashItemInfo.bLocked = false;
	cashItemInfo.Save();

	oPacket.Encode2(ShopInternalPacketFlag::OnCenterMoveToSlotDone);
	oPacket.Encode8(pItem->liItemSN);
	oPacket.Encode1(1);
	oPacket.Encode2(nPOS);
	pItem->RawEncode(&oPacket);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	pItem->Release();
	pSrv->SendPacket(&oPacket);
}

void CharacterDBAccessor::OnCharacterSaveRequest(void *iPacket)
{
	InPacket *iPacket_ = (InPacket*)iPacket;
	GA_Character chr;
	chr.DecodeCharacterData(iPacket_, true);
	chr.Save(false);
	GW_FuncKeyMapped keyMapped(chr.nCharacterID);
	keyMapped.Decode(iPacket_, false);
	keyMapped.Save(false);
}