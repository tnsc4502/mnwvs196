#pragma once

struct GA_Character;
struct GW_ItemSlotBase;
class SecondaryStat;

class BasicStat
{
public:
	enum BasicStatFlag : long long int
	{
		BS_Skin = 0x01,
		BS_Face = 0x02,
		BS_Hair = 0x04,
		BS_Level = 0x10,
		BS_Job = 0x20,
		BS_STR = 0x40,
		BS_DEX = 0x80,
		BS_INT = 0x100,
		BS_LUK = 0x200,
		BS_HP = 0x400,
		BS_MaxHP = 0x800,
		BS_MP = 0x1000,
		BS_MaxMP = 0x2000,
		BS_AP = 0x4000,
		BS_SP = 0x8000,
		BS_EXP = 0x10000,
		BS_POP = 0x20000,
		BS_Meso = 0x40000,
		BS_Pet = 0x180008, //Unk
		BS_GachaponEXP = 0x200000,
		BS_Fatigue = 0x80000,
		BS_Charisma = 0x100000,
		BS_Insight = 0x200000,
		BS_Will = 0x400000,
		BS_Craft = 0x800000,
		BS_Sense = 0x1000000,
		BS_Charm = 0x2000000,
		BS_TrainLimit = 0x4000000
	};

	class BasicStatRateOption
	{
	public:
		int nMMPr = 0,
			nMHPr = 0,
			nLUKr = 0,
			nINTr = 0,
			nDEXr = 0,
			nSTRr = 0;
	};

	BasicStatRateOption *m_bOption;

	int nGender = 0,
		nLevel = 0,
		nJob = 0,
		nSTR = 0,
		nINT = 0,
		nDEX = 0,
		nLUK = 0,
		nPOP = 0,
		nMHP = 0,
		nMMP = 0,
		nBasicStatInc = 0,
		nCharismaEXP = 0,
		nInsightEXP = 0,
		nWillEXP = 0,
		nSenseEXP = 0,
		nCharmEXP = 0,
		nIncMaxHPr = 0,
		nIncMaxMPr = 0,
		nPsdEVAX = 0,
		nIncDEXr = 0,
		nIncSTRr = 0,
		nIncINTr = 0,
		nIncLUKr = 0,
		nSpiritLinkIn = 0,
		nIncMaxHPr_Forced = 0,
		nIncRMAF = 0,
		nIncRMAI = 0,
		nIncRMAL = 0,
		nElemDefault = 0,
		nPDD,
		nMDD,
		nMAD,
		nPAD,
		nACC,
		nEVA,
		nSpeed,
		nJump,
		nCraft
		;

public:
	BasicStat();
	~BasicStat();

	void SetPermanentSkillStat(GA_Character* pChar);
	void SetFrom(GA_Character* pChar, int nMaxHPIncRate, int nMaxMPIncRate, int nBasicStatInc);
};

