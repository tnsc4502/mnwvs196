#pragma once
#include <string>
#include <vector>

struct ActItem;
struct ActSkill;
struct ActQuest;
struct ActSP;

struct QuestAct
{
	int nEXP = 0,
		nCraftEXP = 0,
		nCharismaEXP = 0,
		nCharmEXP = 0,
		nInsightEXP = 0,
		nSenseEXP = 0,
		nWillEXP = 0,
		nPOP = 0,
		nMoney = 0,
		nBuffItemID = 0,
		nTransferField = 0,
		nNextQuest = 0;

	std::vector<ActItem*> aActItem;
	std::vector<ActSkill*> aActSkill;
	std::vector<ActQuest*> aActQuest;
	std::vector<ActSP*> aActSP;

	std::string strPortalName, sInfo;
};

