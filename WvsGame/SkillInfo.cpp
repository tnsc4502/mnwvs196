#include "SkillInfo.h"
#include "..\Database\GA_Character.hpp"
#include "ItemInfo.h"

SkillInfo::SkillInfo()
{
}


SkillInfo::~SkillInfo()
{
}

SkillInfo * SkillInfo::GetInstance()
{
	static SkillInfo* pInstance = new SkillInfo;
	return pInstance;
}

int SkillInfo::GetBundleItemMaxPerSlot(int nItemID, GA_Character * pCharacterData)
{
	auto pItem = ItemInfo::GetInstance()->GetBundleItem(nItemID);
	if (pItem != nullptr)
	{
		int result = pItem->nMaxPerSlot;
		if (pCharacterData != nullptr &&  nItemID / 10000 == 207)
		{
			//ºë·Ç·t¾¹
		}
		return result;
	}
	return 0;
}
