#pragma once

class User;

class QWUser
{
private:
	static bool TryProcessLevelUp(User* pUser, int nInc, int& refReachMaxLvl);

public:
	static long long int IncSTR(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncDEX(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncLUK(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncINT(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncMP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncHP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncMMP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncMHP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncPOP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncSP(User* pUser, int nJobLevel, int nInc, bool bOnlyFull);
	static long long int IncMoney(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncAP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncMaxHPVal(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncMaxMPVal(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncEXP(User* pUser, int nInc, bool bOnlyFull);

	static long long int SetFace(User * pUser, int nFace);
	static long long int SetHair(User * pUser, int nHair);
	static long long int SetJob(User * pUser, int nJob);
	static long long int SetSkin(User * pUser, int nSkin);
};

