#include "SecondaryStat.h"
#include "BasicStat.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Common\Net\OutPacket.h"

SecondaryStat::SecondaryStat()
{
}


SecondaryStat::~SecondaryStat()
{
}

void SecondaryStat::SetFrom(int nFieldType, GA_Character * pChar, BasicStat * pBS, void * pFs, void * pNonBodyEquip, int nMHPForPvP, void * pPSD)
{
	const GW_CharacterStat *pCS = pChar->mStat;

	this->nPAD = 0;
	this->nPDD = 0;

	//不知道新的計算公式為何
	this->nMAD = pBS->nINT;
	this->nMDD = pBS->nINT;

	this->nEVA = pBS->nLUK / 2 + pBS->nDEX / 4;
	this->nACC = pBS->nDEX + pBS->nLUK;
	this->nSpeed = 100;
	this->nJump = 100;
	this->nCraft = pBS->nDEX + pBS->nLUK + pBS->nINT;

	int nPDDIncRate = 0; //shield mastery ?

	const GW_ItemSlotEquip* pEquip;
	for (const auto& itemEquipped : pChar->mItemSlot[1])
	{
		pEquip = (const GW_ItemSlotEquip*)itemEquipped.second;

		nPDD += pEquip->nPDD;
		nPAD += pEquip->nPAD;
		nMDD += pEquip->nMDD;
		nMAD += pEquip->nMAD;

		nACC += pEquip->nACC;
		nEVA += pEquip->nEVA;
	}
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
}

void SecondaryStat::EncodeForLocal(OutPacket * oPacket, TemporaryStat::TS_Flag & flag)
{
	flag.Encode(oPacket);
	if (flag & GET_TS_FLAG(ElementalReset))
	{
		if (EnDecode4Byte(flag))
			oPacket->Encode4(nElementalReset);
		else
			oPacket->Encode2(nElementalReset);
		oPacket->Encode4(rElementalReset);
		oPacket->Encode4(tElementalReset);
	}
	if (flag & GET_TS_FLAG(FireAura))
	{
		if (EnDecode4Byte(flag))
			oPacket->Encode4(nFireAura);
		else
			oPacket->Encode2(nFireAura);
		oPacket->Encode4(rFireAura);
		oPacket->Encode4(tFireAura);
	}
	int nCount = 0;
	oPacket->Encode2(nCount);
	for (int i = 0; i < nCount; ++i)
	{
		oPacket->Encode4(0); //mBuffedForSpecMap
		oPacket->Encode1(0); //bEnable
	}
	oPacket->Encode1(nDefenseAtt);
	oPacket->Encode1(nDefenseState);
	oPacket->Encode1(nPVPDamage);

	oPacket->Encode4(0);

	oPacket->Encode2(1);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	printf("Encode Local TS : \n");
	oPacket->Print();
}

void SecondaryStat::EncodeForRemote(OutPacket * oPacket, TemporaryStat::TS_Flag & flag)
{
}

bool SecondaryStat::EnDecode4Byte(TemporaryStat::TS_Flag & flag)
{
	if ((flag & GET_TS_FLAG(CarnivalDefence))
		|| (flag & GET_TS_FLAG(SpiritLink))
		|| (flag & GET_TS_FLAG(DojangLuckyBonus))
		|| (flag & GET_TS_FLAG(SoulGazeCriDamR))
		|| (flag & GET_TS_FLAG(PowerTransferGauge))
		|| (flag & GET_TS_FLAG(ReturnTeleport))
		|| (flag & GET_TS_FLAG(ShadowPartner))
		|| (flag & GET_TS_FLAG(SetBaseDamage))
		|| (flag & GET_TS_FLAG(QuiverCatridge))
		|| (flag & GET_TS_FLAG(ImmuneBarrier))
		|| (flag & GET_TS_FLAG(NaviFlying))
		|| (flag & GET_TS_FLAG(Dance))
		|| (flag & GET_TS_FLAG(SetBaseDamageByBuff))
		|| (flag & GET_TS_FLAG(DotHealHPPerSecond))
		|| (flag & GET_TS_FLAG(MagnetArea))
		|| (flag & GET_TS_FLAG(RideVehicle)))
		return true;
	printf("EnDecode4Byte [False]\n");
	return false;
}
