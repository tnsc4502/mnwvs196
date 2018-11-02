#pragma once
#include <vector>
#include <map>
#include "Reward.h"
#include "FieldPoint.h"
#include "..\WvsLib\Common\CommonDef.h"

class ReactorTemplate
{
	ALLOW_PRIVATE_ALLOC

	friend class Reactor;
	struct EventInfo
	{
		FieldPoint m_aSpaceVertex[2];
		bool m_bCheckTargetRange = false;
		char m_nTouchable = 0;

		int m_nType = 0,
			m_nStateToBe = 0,
			m_nItemID = 0,
			m_nCount = 0,
			m_tHitDelay = 0;
	};

	struct StateInfo
	{
		std::vector<EventInfo> m_aEventInfo;
		int m_tTimeout = 0,
			m_tHitDelay = 0;
	};

	struct ActionInfo
	{
	};

	std::vector<Reward::RewardInfo> m_aRewardInfo;
	std::vector<StateInfo> m_aStateInfo;
	std::vector<ActionInfo> m_aActionInfo;

	int m_nTemplateID, m_nReqHitCount = 0, m_tHitDelay = 0;
	bool m_bRemoveInFieldSet = false;

	ReactorTemplate();
	static std::map<int, ReactorTemplate*> m_mReactorTemplate;
public:

	static void RegisterReactor(int nTemplateID, void *pImg);
	static void LoadEvent(StateInfo* pInfo, void * pImg);
	static void Load();
	static ReactorTemplate* GetReactorTemplate(int nTemplateID);

	StateInfo* GetStateInfo(int nState);
	EventInfo* GetEventInfo(int nState, int nEventIdx);
};

