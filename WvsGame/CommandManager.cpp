#include "CommandManager.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "ItemInfo.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBundle.h"
#include "User.h"
#include "Field.h"
#include "Reward.h"
#include "DropPool.h"
#include <vector>

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

void Split(const std::string& str, std::vector<std::string>& result)
{
	int prevPos = 0, nextPos = 0, size = (int)str.size();
	while ((nextPos = str.find(" ", prevPos)) >= 0 && nextPos < size)
	{
		result.push_back(std::move(str.substr(prevPos, nextPos - prevPos)));
		prevPos = nextPos + 1;
	}
	result.push_back(std::move(str.substr(prevPos, size - prevPos)));
}

void CommandManager::Process(User * pUser, const std::string & input)
{
	std::vector<std::string> token;
	Split(input, token);
	for (const auto& tkn : token)
		printf("Token Result : %s\n", tkn.c_str());

	if (token.size() >= 2 && token[0][0] == '#')
	{
		if (token[0] == "#item")
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
						0,
						0,
						0);
				}
			}
		}
	}
}
