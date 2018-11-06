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

	static long long int GetSTR(User* pUser);
	static long long int GetDEX(User* pUser);
	static long long int GetLUK(User* pUser);
	static long long int GetINT(User* pUser);
	static long long int GetMP(User* pUser);
	static long long int GetHP(User* pUser);
	static long long int GetMMP(User* pUser);
	static long long int GetMHP(User* pUser);
	static long long int GetPOP(User* pUser);
	static long long int GetSP(User* pUser, int nJobLevel);
	static long long int GetMoney(User* pUser);
	static long long int GetAP(User* pUser);
	static long long int GetMaxHPVal(User* pUser);
	static long long int GetMaxMPVal(User* pUser);
	static long long int GetEXP(User* pUser);

	static long long int SetFace(User * pUser, int nFace);
	static long long int SetHair(User * pUser, int nHair);
	static long long int SetJob(User * pUser, int nJob);
	static long long int SetSkin(User * pUser, int nSkin);
};

