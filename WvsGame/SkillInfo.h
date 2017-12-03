#pragma once
class GA_Character;

class SkillInfo
{
public:
	SkillInfo();
	~SkillInfo();

	static SkillInfo* GetInstance();
	int GetBundleItemMaxPerSlot(int nItemID, GA_Character* pCharacterData);
};

