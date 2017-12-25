#include "BasicStat.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"
#include "SecondaryStat.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "ItemInfo.h"

BasicStat::BasicStat()
{
	m_bOption = new BasicStatRateOption;
}


BasicStat::~BasicStat()
{
	delete m_bOption;
}

void BasicStat::SetFrom(int nFieldType, GA_Character * pChar, SecondaryStat * pSS, void * pFs, void * pNonBodyEquip, int nMHPForPvP, void * pPSD)
{
	GW_CharacterStat *pCS = pChar->mStat;

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

	SkillEntry
		*pShoutOfEmpress = nullptr,
		*pMichaelShoutOfEmpress = nullptr,
		*pUltimateAdventurer = nullptr,
		*pReinforcementOfEmpress = nullptr;

	int nCheckSLV = 0;
	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 10000074, &pShoutOfEmpress, 0, 0, 0, 1);
	if (nCheckSLV && pShoutOfEmpress)
	{
		auto pLevelData = pShoutOfEmpress->GetLevelData(nCheckSLV);
		nIncMaxHPr = pLevelData->m_nX;
		nIncMaxMPr = pLevelData->m_nX;
	}

	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 50000074, &pMichaelShoutOfEmpress, 0, 0, 0, 1);
	if (nCheckSLV && pMichaelShoutOfEmpress)
	{
		auto pLevelData = pMichaelShoutOfEmpress->GetLevelData(nCheckSLV);
		nIncMaxHPr = pLevelData->m_nX;
		nIncMaxMPr = pLevelData->m_nX;
	}

	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 74, &pUltimateAdventurer, 0, 0, 0, 1);
	if (nCheckSLV && pUltimateAdventurer)
	{
		auto pLevelData = pUltimateAdventurer->GetLevelData(nCheckSLV);
		nLevel = pLevelData->m_nX;
	}

	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 80, &pReinforcementOfEmpress, 0, 0, 0, 1);
	if (nCheckSLV && pReinforcementOfEmpress)
	{
		auto pLevelData = pReinforcementOfEmpress->GetLevelData(nCheckSLV);
		nLevel = pLevelData->m_nX;
	}

	nIncMaxHPr_Forced = nIncMaxHPr;

	for (auto& itemEquipped : pChar->mItemSlot[1])
	{
		//std::map是ordered, >= 0的表示還放在背包裡面且後面出現的物品 POS 一定>=0，故終止
		auto pItem = itemEquipped.second;
		if (pItem->nPOS >= 0)
			break;
		if (pItem->nPOS == -11)
		{
			auto pEquipItem = ItemInfo::GetInstance()->GetEquipItem(pItem->nItemID);
			//pEquipItem->nRMAF
		}

	}
}
