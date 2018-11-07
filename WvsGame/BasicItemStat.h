#pragma once

/*
該物品造成的能力提升屬性 -- 正服沒有這個class
*/
struct BasicIncrementStat
{
	//增加 inc
	int	niSTR = 0,
		niDEX = 0,
		niINT = 0,
		niLUK = 0,
		niMaxHP = 0,
		niMaxMP = 0,
		niPAD = 0,
		niMAD = 0,
		niPDD = 0,
		niMDD = 0,
		niACC = 0,
		niEVA = 0,
		niCraft = 0,
		niSpeed = 0,
		niJump = 0,
		niSwim = 0,
		niFatigue = 0;
};

/*
該物品的限制屬性，例如可否交易等 -- 正服沒有這個class
*/
struct BasicAbilityStat
{
	int nAttribute = 0;

	bool bTimeLimited = false,
		bCash = false,
		bPartyQuest = false,
		bBigSize = false;
};