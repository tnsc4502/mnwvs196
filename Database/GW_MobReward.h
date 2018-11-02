#pragma once
#include <vector>
#include <map>
#include "..\WvsLib\Common\CommonDef.h"

class GW_MobReward
{
	ALLOW_PRIVATE_ALLOC

	GW_MobReward();
	~GW_MobReward();
	struct RewardInfo
	{
		int nItemID,
			nCountMin,
			nCountMax,
			nWeight;

		short nQRecord;

		long long int liExpired;
	};

	//給Manager用
	static std::map<int, std::vector<RewardInfo*>> m_mReward;

	//每個怪物自己持有，指向m_mReward中
	std::vector<RewardInfo*> m_aReward;

	int m_nTotalWeight = 0;

public:
	static GW_MobReward* GetInstance();
	void Load();
	GW_MobReward* GetMobReward(int nMobID);
	const std::vector<RewardInfo*>& GetRewardList() const;
	const int GetTotalWeight() const;
};

