#include "BasicStat.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"

#include "SecondaryStat.h"
#include "ItemInfo.h"

BasicStat::BasicStat()
{
	m_bOption = new BasicStatRateOption;
}


BasicStat::~BasicStat()
{
	delete m_bOption;
}

void BasicStat::SetFrom(int nFieldType, GA_Character * pChar, BasicStat* pSS_, void * pFs, void * pNonBodyEquip, int nMHPForPvP, void * pPSD)
{
	const SecondaryStat* pSS = (const SecondaryStat*)pSS_;
	const GW_CharacterStat *pCS = pChar->mStat;

	nGender = pChar->nGender;
	nLevel = pChar->mLevel->nLevel;
	nJob = pCS->nJob;
	nSTR = pCS->nStr;
	nINT = pCS->nInt;
	nDEX = pCS->nDex;
	nLUK = pCS->nLuk;
	nPOP = pCS->nPOP;
	nMHP = pCS->nMaxHP;
	nMMP = pCS->nMaxMP;
	nCharismaEXP = pCS->nCharismaEXP;
	nInsightEXP = pCS->nInsightEXP;
	nWillEXP = pCS->nWillEXP;
	nSenseEXP = pCS->nSenseEXP;
	nCharmEXP = pCS->nCharmEXP;

	const GW_ItemSlotEquip* pEquip;
	for (const auto& itemEquipped : pChar->mItemSlot[1])
	{
		pEquip = (const GW_ItemSlotEquip*)itemEquipped.second;
		nSTR += pEquip->nSTR;
		nLUK += pEquip->nLUK;
		nDEX += pEquip->nDEX;
		nINT += pEquip->nINT;
		
		nMHP += pEquip->nMaxHP;
		nMMP += pEquip->nMaxMP;

		//Apply item option here.
	}
}
