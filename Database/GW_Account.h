#pragma once
struct GW_Account
{
	int nAccountID = 0,
		nNexonCash = 0,
		nMaplePoint = 0;

#ifdef DBLIB
	void Load(int nAccount);
#endif

	int QueryCash(int nChargeType);
	void UpdateCash(int nChargeType, int nCharge);

	GW_Account();
	~GW_Account();
};

