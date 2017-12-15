#pragma once
#include <map>
#include <mutex>
#include <atomic>

struct GA_Character;
class SkillEntry;

#define PARSE_SKILLDATA(attribute) parser.compile(#attribute, expression), expression.value();

class SkillInfo
{
	std::atomic<int> m_nOnLoadingSkills;
	std::mutex m_mtxSkillResLock;
	std::map<int, std::map<int, SkillEntry*> *> m_mSkillByRootID;

public:
	SkillInfo();
	~SkillInfo();

	int GetLoadingSkillCount() const;

	static SkillInfo* GetInstance();
	int GetBundleItemMaxPerSlot(int nItemID, GA_Character* pCharacterData);
	void IterateSkillInfo();
	void LoadSkillRoot(int nSkillRootID, void* pData);
	SkillEntry* LoadSkill(int nSkillRootID, int nSkillID, void* pData);
	void LoadLevelData(int nSkillID, SkillEntry* pEntry, void* pData);
};

