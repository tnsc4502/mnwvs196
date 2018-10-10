#pragma once
#include <vector>
#include <map>
#include "Reward.h"

class ReactorTemplate
{
	struct EventInfo
	{

	};

	struct StateInfo
	{
		std::vector<EventInfo> m_aEventInfo;
	};

	struct ActionInfo
	{
	};

	std::vector<Reward::RewardInfo> m_aRewardInfo;
	std::vector<StateInfo> m_aStateInfo;
	std::vector<ActionInfo> m_aActionInfo;

	ReactorTemplate();
	static std::map<int, ReactorTemplate*> m_mReactorTemplate;
public:

	static void RegisterReactor(int nTemplateID, void *pImg);
	static void Load();
};

