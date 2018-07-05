#pragma once
#include <map>
#include <set>

struct QuestAct;
struct QuestDemand;

class User;

class QuestMan
{
	std::map<int, QuestAct*> m_mStartAct, m_mCompleteAct;
	std::map<int, QuestDemand*> m_mStartDemand, m_mCompleteDemand;
	std::set<int> m_mAutoStartQuest, m_mAutoCompleteQuest;

	QuestMan();
	~QuestMan();
public:
	void LoadAct();
	void LoadDemand();
	void RegisterAct(void* pProp);
	void RegisterDemand(void* pProp);
	static QuestMan* GetInstance();

	bool IsAutoStartQuest(int nQuestID);
	bool IsAutoCompleteQuest(int nQuestID);
	bool CheckStartDemand(int nQuestID, User *pUser);
	bool CheckCompleteDemand(int nQuestID, User *pUser);

	QuestAct* GetStartAct(int nQuestID);
	QuestAct* GetCompleteAct(int nQuestID);
};

