#include "CharacterDBAccessor.h"
#include "..\Common\Net\OutPacket.h"
#include "..\Common\Net\SocketBase.h"
#include "..\Common\Net\PacketFlags\CenterPacketFlags.hpp"

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
	oPacket.Encode2(CenterPacketFlag::CharacterListResponse);
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
	
	chrEntry.mAvatarData->nFace = nFace;
	chrEntry.mAvatarData->nHair = nHair;
	chrEntry.mAvatarData->nSkin = nSkin;

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

	GW_ItemSlotEquip hatEquip;
	hatEquip.nItemID = aBody[EQP_ID_HatEquip];
	hatEquip.nPOS = EQP_POS_Hat;

	GW_ItemSlotEquip topEquip;
	topEquip.nItemID = aBody[EQP_ID_TopEquip];
	topEquip.nPOS = EQP_POS_Top;

	GW_ItemSlotEquip bottomEquip;
	bottomEquip.nItemID = aBody[EQP_ID_ButtomEquip];
	bottomEquip.nPOS = EQP_POS_Bottom;

	GW_ItemSlotEquip weaponEquip;
	weaponEquip.nItemID = aBody[EQP_ID_WeaponEquip];
	weaponEquip.nPOS = EQP_POS_Weapon;

	GW_ItemSlotEquip shoesEquip;
	shoesEquip.nItemID = aBody[EQP_ID_ShoesEquip];
	shoesEquip.nPOS = EQP_POS_Shoes;

	GW_ItemSlotEquip capeEquip;
	capeEquip.nItemID = aBody[EQP_ID_CapeEquip];
	capeEquip.nPOS = EQP_POS_Cape;

	GW_ItemSlotEquip shieldEquip;
	shieldEquip.nItemID = aBody[EQP_ID_ShieldEquip];
	shieldEquip.nPOS = EQP_POS_Shield;

	GW_ItemSlotEquip* equips[EQP_ID_FLAG_END] = {
		&hatEquip,
		&topEquip,
		&bottomEquip,
		&weaponEquip,
		&shoesEquip,
		&capeEquip,
		&shieldEquip
	};
	int nEquipCount = sizeof(equips) / sizeof(GW_ItemSlotBase*);
	for (int i = 0; i < nEquipCount; ++i)
		if (equips[i]->nItemID > 0)
			chrEntry.aEquipItem.push_back(*(equips[i]));
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

void CharacterDBAccessor::PostCharacterDataRequest(SocketBase *pSrv, int nClientSocketID, int nCharacterID)
{
	GA_Character chrEntry;
	chrEntry.Load(nCharacterID);
	OutPacket oPacket;
	oPacket.Encode2(CenterPacketFlag::CenterMigrateInResult);
	oPacket.Encode4(nClientSocketID);
	chrEntry.EncodeCharacterData(&oPacket);
	pSrv->SendPacket(&oPacket);
}