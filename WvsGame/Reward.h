#pragma once
#include <memory>

struct GW_ItemSlotBase;

class Reward
{
public:
	struct RewardInfo
	{
		unsigned short m_usQRKey;
		int m_nMaxCount;
	};

private:
	GW_ItemSlotBase* m_pItem;

	int m_nMoney = 0, 
		m_nPeriod = 0,
		m_nType = 0;

	std::shared_ptr<RewardInfo> m_pInfo;

public:
	Reward();
	Reward(Reward* pOther);
	~Reward();

	int GetType() { return m_nType; }
	int GetMoney() { return m_nMoney; }
	int GetPeriod() { return m_nPeriod; }

	void SetType(int nType) { m_nType = nType; }
	void SetMoney(int nMoney) { m_nMoney = nMoney; }
	void SetPeriod(int nPeriod) { m_nPeriod = nPeriod; }

	GW_ItemSlotBase* GetItem() { return m_pItem; }
	void SetItem(GW_ItemSlotBase* pItem) { m_pItem = pItem; }
	std::shared_ptr<Reward::RewardInfo> GetRewardInfo() { return m_pInfo; }
};

