#pragma once
#include <string>
#include "BasicItemStat.h"

struct EquipItem
{
	std::string sItemName;
	BasicAbilityStat abilityStat;
	BasicIncrementStat incStat;

	//能力需求 request
	int nItemID,
		nrSTR,
		nrINT,
		nrDEX,
		nrLUK,
		nrPOP,
		nrJob,
		nrLevel,
		nrMobLevel,
		nRUC,
		nSellPrice,
		nSwim,
		nTamingMob,
		nKnockBack,
		nIncRMAF,
		nIncRMAI,
		nIncRMAL,
		nElemDefault,
		nCuttable,
		//使用此裝備對寵物的影響
		dwPetAbilityFlag;
};

