#include "QWUser.h"
#include "User.h"
#include "SecondaryStat.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_Avatar.hpp"

bool QWUser::TryProcessLevelUp(User * pUser, int nInc, int & refReachMaxLvl)
{
	if (WvsGameConstants::m_nEXP[1] == 0)
		WvsGameConstants::LoadEXP();
	auto& ref = pUser->GetCharacterData()->mStat->nExp;
	ref += nInc;
	if (pUser->GetCharacterData()->mLevel->nLevel < 250 &&
		ref >= WvsGameConstants::m_nEXP[pUser->GetCharacterData()->mLevel->nLevel])
	{
		ref -= WvsGameConstants::m_nEXP[pUser->GetCharacterData()->mLevel->nLevel];
		++pUser->GetCharacterData()->mLevel->nLevel;
		if (pUser->GetCharacterData()->mLevel->nLevel >= 250)
			refReachMaxLvl = 1;
		return true;
	}
	return false;
}

long long int QWUser::IncSTR(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& refValue = pUser->GetCharacterData()->mStat->nStr;
	if (bOnlyFull && refValue + nInc < 0 || refValue + nInc > 999)
		return 0;
	refValue += nInc;
	return BasicStat::BS_STR;
}

long long int QWUser::IncDEX(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& refValue = pUser->GetCharacterData()->mStat->nDex;
	if (bOnlyFull && refValue + nInc < 0 || refValue + nInc > 999)
		return 0;
	refValue += nInc;
	return BasicStat::BS_DEX;
}

long long int QWUser::IncLUK(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& refValue = pUser->GetCharacterData()->mStat->nLuk;
	if (bOnlyFull && refValue + nInc < 0 || refValue + nInc > 999)
		return 0;
	refValue += nInc;
	return BasicStat::BS_LUK;
}

long long int QWUser::IncINT(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& refValue = pUser->GetCharacterData()->mStat->nInt;
	if (bOnlyFull && refValue + nInc < 0 || refValue + nInc > 999)
		return 0;
	refValue += nInc;
	return BasicStat::BS_INT;
}

long long int QWUser::IncMP(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	if (nInc < 0 && pUser->GetSecondaryStat()->nInfinity > 0)
		return 0;
	int nMP = pUser->GetCharacterData()->mStat->nMP;
	int nMaxMP = pUser->GetBasicStat()->nMMP;

	nMP += nInc;
	if (nMP < 0)
		nMP = 0;
	if (nMP >= nMaxMP)
		nMP = nMaxMP;
	pUser->GetCharacterData()->mStat->nMP = nMP;

	return BasicStat::BS_MP;
}

long long int QWUser::IncHP(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nHP = pUser->GetCharacterData()->mStat->nHP;
	int nMaxHP = pUser->GetBasicStat()->nMHP;

	nHP += nInc;
	if (nHP < 0)
		nHP = 0;
	if (nHP >= nMaxHP)
		nHP = nMaxHP;
	pUser->GetCharacterData()->mStat->nHP = nHP;

	return BasicStat::BS_HP;
}

long long int QWUser::IncMMP(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nMaxMP = pUser->GetCharacterData()->mStat->nMaxMP;
	nMaxMP += nInc;
	if (nMaxMP <= 5)
		nMaxMP = 5;
	if (nMaxMP >= 90000)
		nMaxMP = 90000;
	pUser->GetCharacterData()->mStat->nMaxMP = nMaxMP;

	return BasicStat::BS_MaxMP;
}

long long int QWUser::IncMHP(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nMaxHP = pUser->GetCharacterData()->mStat->nMaxHP;
	nMaxHP += nInc;
	if (nMaxHP <= 5)
		nMaxHP = 5;
	if (nMaxHP >= 90000)
		nMaxHP = 90000;
	pUser->GetCharacterData()->mStat->nMaxHP = nMaxHP;

	return BasicStat::BS_MaxHP;
}

long long int QWUser::IncPOP(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nPOP = pUser->GetCharacterData()->mStat->nPOP;
	nPOP += nInc;
	if (bOnlyFull && (nPOP > 30000 || nPOP < -30000))
		return 0;
	if (nPOP > 30000)
		nPOP = 30000;
	if (nPOP < -30000)
		nPOP = -30000;
	pUser->GetCharacterData()->mStat->nPOP = nPOP;

	return BasicStat::BS_POP;
}

long long int QWUser::IncSP(User * pUser, int nJobLevel, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	if (nJobLevel < 0 || nJobLevel >= GW_CharacterStat::EXTEND_SP_SIZE)
		return 0;
	auto& ref = pUser->GetCharacterData()->mStat->aSP[nJobLevel];
	if (bOnlyFull && (ref + nInc < 0 || ref + nInc > 255))
		return 0;
	ref += nInc;
	if (ref < 0)
		ref = 0;
	if (ref >= 255)
		ref = 255;
	return BasicStat::BS_SP;
}

long long int QWUser::IncMoney(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mMoney->nMoney += nInc;
	return BasicStat::BS_Meso;
}

long long int QWUser::IncAP(User * pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nAP = pUser->GetCharacterData()->mStat->nAP;
	nAP += nInc;
	if (bOnlyFull && (nAP > 255 || nAP < 0))
		return 0;
	if (nAP > 255)
		nAP = 255;
	if (nAP < 0)
		nAP = 0;
	pUser->GetCharacterData()->mStat->nAP = nAP;

	return BasicStat::BS_AP;
}

long long int QWUser::IncMaxHPVal(User * pUser, int nInc, bool bOnlyFull)
{
	return false;
}

long long int QWUser::IncMaxMPVal(User * pUser, int nInc, bool bOnlyFull)
{
	return false;
}

long long int QWUser::IncEXP(User * pUser, int nInc, bool bOnlyFull)
{
	long long int nRet= BasicStat::BS_EXP;
	int nReachMaxLvl = 0;
	if (TryProcessLevelUp(pUser, nInc, nReachMaxLvl))
	{
		nRet |= BasicStat::BS_Level;
		nRet |= IncAP(
			pUser,
			WvsGameConstants::IsCygnusJob(pUser->GetCharacterData()->mStat->nJob) ? 6 : 5,
			false);
		nRet |= IncSP(
			pUser,
			WvsGameConstants::GetJobLevel(pUser->GetCharacterData()->mStat->nJob),
			3,
			false);
		pUser->ValidateStat();
		pUser->OnLevelUp();
	}
	return nRet;
}

long long int QWUser::GetSTR(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nSTR;
}

long long int QWUser::GetDEX(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nDEX;
}

long long int QWUser::GetLUK(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nLUK;
}

long long int QWUser::GetINT(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nINT;
}

long long int QWUser::GetMP(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nMP;
}

long long int QWUser::GetHP(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nHP;
}

long long int QWUser::GetMMP(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nMMP;
}

long long int QWUser::GetMHP(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nMHP;
}

long long int QWUser::GetPOP(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nFame;
}

long long int QWUser::GetSP(User * pUser, int nJobLevel)
{
	return pUser->GetCharacterData()->mStat->aSP[nJobLevel];
}

long long int QWUser::GetMoney(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mMoney->nMoney;
}

long long int QWUser::GetAP(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nAP;
}

long long int QWUser::GetMaxHPVal(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nMHP;
}

long long int QWUser::GetMaxMPVal(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nMMP;
}

long long int QWUser::GetEXP(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nExp;
}

long long int QWUser::SetFace(User * pUser, int nFace)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mStat->nFace = nFace;
	pUser->GetCharacterData()->mAvatarData->nFace = nFace;
	return BasicStat::BS_Face;
}

long long int QWUser::SetHair(User * pUser, int nHair)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mStat->nHair = nHair;
	pUser->GetCharacterData()->mAvatarData->nHair = nHair;
	return BasicStat::BS_Hair;
}

long long int QWUser::SetJob(User * pUser, int nJob)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mStat->nJob = nJob;
	return BasicStat::BS_Job;
}

long long int QWUser::SetSkin(User * pUser, int nSkin)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mStat->nSkin = nSkin;
	pUser->GetCharacterData()->mAvatarData->nSkin = nSkin;
	return BasicStat::BS_Skin;
}
