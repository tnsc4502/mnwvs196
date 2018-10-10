#pragma once

/*
該物品造成的能力提升屬性 -- 正服沒有這個class
*/
struct BasicIncrementStat
{
	//增加 inc
	int	niSTR,
		niDEX,
		niINT,
		niLUK,
		niMaxHP,
		niMaxMP,
		niPAD,
		niMAD,
		niPDD,
		niMDD,
		niACC,
		niEVA,
		niCraft,
		niSpeed,
		niJump,
		niSwim,
		niFatigue;
};

/*
該物品的限制屬性，例如可否交易等 -- 正服沒有這個class
*/
struct BasicAbilityStat
{
	int nAttribute;

	bool bTimeLimited,
		bCash,
		bPartyQuest,
		bBigSize;
};