#include "CharacterDBAccessor.h"
#include "GW_ItemSlotEquip.h"
#include "GW_ItemSlotBundle.h"
#include "GW_CharacterStat.h"
#include "GW_CharacterLevel.h"
#include "GW_Avatar.hpp"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"

#include "GW_CharacterList.hpp"
#include "GA_Character.hpp"

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
	chrEntry.nGender = nGender;

	chrEntry.nFieldID = 100000000;
	chrEntry.nGuildID = chrEntry.nPartyID = chrEntry.nFame = 0;
	
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
			chrEntry.mItemSlot[1].insert({ equips[i]->nPOS, equips[i] });
	chrEntry.Save(true);
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
	chrEntry.EncodeCharacterData(oPacket, true);
}

void CharacterDBAccessor::OnCharacterSaveRequest(void *iPacket)
{
	InPacket *iPacket_ = (InPacket*)iPacket;
	GA_Character chr;
	chr.DecodeCharacterData(iPacket_, true);
	chr.Save(false);
}