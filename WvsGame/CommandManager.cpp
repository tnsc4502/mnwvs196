#include "CommandManager.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "ItemInfo.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_CharacterStat.h"
#include "User.h"
#include "USkill.h"
#include "Field.h"
#include "Reward.h"
#include "Summoned.h"
#include "Mob.h"
#include "MobTemplate.h"
#include "LifePool.h"
#include "DropPool.h"
#include <vector>
#include "..\WvsLib\String\StringUtility.h"
#include "..\WvsLib\DateTime\GameDateTime.h"

CommandManager::CommandManager()
{
}


CommandManager::~CommandManager()
{
}

CommandManager * CommandManager::GetInstance()
{
	static CommandManager* pInstance = new CommandManager;
	return pInstance;
}

void CommandManager::Process(User * pUser, const std::string & input)
{
	std::vector<std::string> token;
	StringUtility::Split(input, token, " ");

	if (token.size() >= 1 && token[0][0] == '#')
	{
		for (int i = 0; i < token[0].size(); ++i)
			token[0][i] = tolower(token[0][i]);
		auto& sCmd = token[0];
		if (sCmd == "#mov")
			Summoned::SUMMONED_MOV = atoi(token[1].c_str());
		else if(sCmd == "#att")
			Summoned::SUMMONED_ATT = atoi(token[1].c_str());
		else if (sCmd == "#item")
		{
			int nItemID = atoi(token[1].c_str());
			if (nItemID / 1000000 >= 1)
			{
				auto pNewItem = ItemInfo::GetInstance()->GetItemSlot(nItemID, ItemInfo::ItemVariationOption::ITEMVARIATION_NORMAL);
				if (pNewItem)
				{
					if (pNewItem->nType != GW_ItemSlotBase::GW_ItemSlotType::EQUIP
						&& pNewItem->nType != GW_ItemSlotBase::GW_ItemSlotType::CASH
						&& token.size() >= 3)
						((GW_ItemSlotBundle*)pNewItem)->nNumber = atoi(token[2].c_str());
						
					//pNewItem->liExpireDate = GameDateTime::GetDateExpireFromPeriod(1);
					Reward reward;
					reward.SetType(1);
					reward.SetItem(pNewItem);
					pUser->GetField()->GetDropPool()->Create(
						&reward,
						0,
						0,
						0,
						0,
						pUser->GetPosX(),
						pUser->GetPosY(),
						pUser->GetPosX(),
						pUser->GetPosY(),
						0,
						1,
						0,
						0);
				}
			}
		}
		else if (sCmd == "#maxskill")
		{
			int nJob = pUser->GetCharacterData()->mStat->nJob;
			int nLevel = nJob % 10;
			while (nLevel >= 0) 
			{
				auto pSkills = SkillInfo::GetInstance()->GetSkillsByRootID(nJob - (2 - nLevel));
				for (const auto& pSkill : *pSkills)
				{
					auto pEntry = pSkill.second;
					auto pMaxLevelData = pEntry->GetLevelData(pEntry->GetMaxLevel());
					USkill::OnSkillUpRequest(
						pUser,
						pEntry->GetSkillID(),
						pEntry->GetMaxLevel(),
						false,
						false);
				}
				--nLevel;
			}

			//Beginner
			auto pSkills = SkillInfo::GetInstance()->GetSkillsByRootID((nJob / 100) * 100);
			for (const auto& pSkill : *pSkills)
			{
				auto pEntry = pSkill.second;
				auto pMaxLevelData = pEntry->GetLevelData(pEntry->GetMaxLevel());
				USkill::OnSkillUpRequest(
					pUser,
					pEntry->GetSkillID(),
					pEntry->GetMaxLevel(),
					false,
					false);
			}
		}
		else if (sCmd == "#transfer")
		{
			int nFieldID = atoi(token[1].c_str());
			pUser->TryTransferField(
				nFieldID,
				""
			);
		}
		else if (sCmd == "#mob")
		{
			int nTemplateID = atoi(token[1].c_str());

			Mob mob;
			mob.SetPosX(pUser->GetPosX());
			mob.SetPosY(pUser->GetPosY());
			mob.SetTemplateID(nTemplateID);
			mob.SetMobTemplate(MobTemplate::GetMobTemplate(mob.GetTemplateID()));
			pUser->GetField()->GetLifePool()->CreateMob(
				mob,
				pUser->GetPosX(),
				pUser->GetPosY(),
				pUser->GetFh(),
				0,
				-2,
				0,
				0,
				0,
				nullptr
			);
		}
	}
}
