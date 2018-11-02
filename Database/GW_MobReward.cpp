#include "GW_MobReward.h"
#include "WvsUnified.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

std::map<int, std::vector<GW_MobReward::RewardInfo*>> GW_MobReward::m_mReward;

GW_MobReward::GW_MobReward()
{
}


GW_MobReward::~GW_MobReward()
{
}

GW_MobReward * GW_MobReward::GetInstance()
{
	static GW_MobReward* pInstance = new GW_MobReward;
	return pInstance;
}

void GW_MobReward::Load()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM MobRewards";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	for (auto& result : recordSet)
	{
		RewardInfo* pInfo = AllocObj(RewardInfo);
		pInfo->nItemID = (int)result["itemid"];
		pInfo->nCountMin = (int)result["minimum_quantity"];
		pInfo->nCountMax = (int)result["maximum_quantity"];
		pInfo->nQRecord = (int)result["questid"];
		pInfo->nWeight = (int)result["chance"];
		m_mReward[(int)result["dropperid"]].push_back(pInfo);
	}
}

GW_MobReward * GW_MobReward::GetMobReward(int nMobID)
{
	auto findIter = m_mReward.find(nMobID);
	if (findIter == m_mReward.end())
		return nullptr;

	GW_MobReward* pReward = AllocObj(GW_MobReward);
	pReward->m_aReward = findIter->second;
	if (pReward->m_nTotalWeight == 0)
		for (auto& pInfo : pReward->m_aReward)
			pReward->m_nTotalWeight += pInfo->nWeight;
	return pReward;
}

const std::vector<GW_MobReward::RewardInfo*>& GW_MobReward::GetRewardList() const
{
	return m_aReward;
}

const int GW_MobReward::GetTotalWeight() const
{
	return m_nTotalWeight;
}
