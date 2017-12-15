#pragma once
#include <vector>

struct SkillLevelData;

class SkillEntry
{
	std::vector<SkillLevelData*> m_aLevelData;

	int m_nMasterLevel, m_nMaxLevel, m_nSkillID;

public:
	SkillEntry();
	~SkillEntry();

	void AddLevelData(SkillLevelData* pLevelData);
	const SkillLevelData* GetLevelData(int nLevel) const;
	const std::vector<SkillLevelData*>& GetAllLevelData() const;

	void SetMasterLevel(int nLevel);
	void SetMaxLevel(int nLevel);
	void SetSkillID(int nID);

	int GetMasterLevel() const;
	int GetMaxLevel() const;
	int GetSkillID() const;
};

