#pragma once
#include <vector>
#include <map>

struct QuestDemand
{
	//key = nItemID  Value = nCount
	std::map<int, int> m_mDemandItem;

	//key = nItemID  Value = nState
	std::map<int, int> m_mDemandQuest;

	//key = nSkillID  Value = nAcquire
	std::map<int, int> m_mDemandSkill;

	//key = nMobID  Value = nCount
	std::map<int, int> m_mDemandMob;

	std::vector<int> m_aDemandPet, m_aDemandJob, m_aFieldEnter;

	int nPartyQuest_S = 0,
		nDayByDay = 0,
		nNormalAutoStart = 0,
		nSubJobFlags = 0,
		nPetTamenessMin = 0,
		nNpc = 0,
		nQuestComplete = 0,
		nPOP = 0,
		nInterval = 0,
		nLVMax = 0,
		nLVMin = 0;

	std::string sStartScript, sEndScript;
};

