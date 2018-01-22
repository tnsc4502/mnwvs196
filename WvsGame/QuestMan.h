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
	std::set<int> m_mAutoCompleteQuest;

	QuestMan();
	~QuestMan();
public:
	void LoadAct();
	void LoadDemand();
	void RegisterAct(void* pProp);
	void RegisterDemand(void* pProp);
	static QuestMan* GetInstance();

	bool IsAutoCompleteQuest(int nQuestID);
	bool CheckStartDemand(int nQuestID, User *pUser);
};

