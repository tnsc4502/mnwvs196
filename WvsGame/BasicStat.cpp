#include "BasicStat.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"

#include "SecondaryStat.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "..\Database\GW_SkillRecord.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\WvsGameConstants.hpp"

BasicStat::BasicStat()
{
	m_bOption = AllocObj( BasicStatRateOption );
}


BasicStat::~BasicStat()
{
	FreeObj( m_bOption );
}

void BasicStat::SetPermanentSkillStat(GA_Character * pChar)
{
	int nMaxHPIncRate = 0, nMaxMPIncRate = 0;

	for (auto& skillRecord : pChar->mSkillRecord)
	{
		auto pEntry = SkillInfo::GetInstance()->GetSkillByID(skillRecord.first);
		if (pEntry) 
		{
			auto pLevelData = pEntry->GetLevelData(skillRecord.second->nSLV);
			if (!pLevelData)
				continue;

			//Add stat by Character's level
			if (pLevelData->m_nLv2mhp)
				nMHP += (pChar->mLevel->nLevel) * pLevelData->m_nLv2mhp;
			if (pLevelData->m_nLv2mmp)
				nMMP += (pChar->mLevel->nLevel) * pLevelData->m_nLv2mmp;

			nMaxHPIncRate += pLevelData->m_nMhpR;
			nMaxMPIncRate += pLevelData->m_nMmpR;
		}
	}
	nMHP = nMHP * (nMaxHPIncRate + 100) / 100;
	nMMP = nMMP * (nMaxMPIncRate + 100) / 100;
	if (nMHP > 500000)
		nMHP = 500000;
	if (nMMP > 500000)
		nMMP = 500000;
}

void BasicStat::SetFrom(GA_Character * pChar, int nMaxHPIncRate, int nMaxMPIncRate, int nBasicStatInc)
{
	const GW_CharacterStat *pCS = pChar->mStat;

	nGender = pChar->mStat->nGender;
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
	SetPermanentSkillStat(pChar);
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
	nMHP = nMHP * (nMaxHPIncRate + 100) / 100;
	nMMP = nMMP * (nMaxMPIncRate + 100) / 100;
	if (nMHP > 500000)
		nMHP = 500000;
	if (nMMP > 500000)
		nMMP = 500000;
}
