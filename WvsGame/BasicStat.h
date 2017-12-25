#pragma once

struct GA_Character;
struct GW_ItemSlotBase;
class SecondaryStat;

class BasicStat
{
	class BasicStatRateOption
	{
		friend class BasicStat;
		int nMMPr = 0,
			nMHPr = 0,
			nLUKr = 0,
			nINTr = 0,
			nDEXr = 0,
			nSTRr = 0;
	};

private:
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
		nElemDefault = 0
		;

public:
	BasicStat();
	~BasicStat();

	void SetFrom(int nFieldType, GA_Character* pChar, SecondaryStat* pSS, void *pFs, void* pNonBodyEquip, int nMHPForPvP, void* pPSD);
};

