#pragma once
#include <map>
#include <vector>
#include "CSCommodity.h"

struct GW_CashItemInfo;
struct CSCommodity;

class ShopInfo
{
	std::map<int, CSCommodity> m_mOriginalCommodity, m_mCommodity;

public:
	ShopInfo();
	~ShopInfo();

	static ShopInfo* GetInstance();
	const CSCommodity* GetCSCommodity(int nSN) const;
	void LoadCommodity(void *pCashPackage, bool bCheckValid);
	GW_CashItemInfo* GetCashItemInfo(const CSCommodity *pCS) const;
	void Init();
};

