#include "StateChangeItem.h"
#include <mutex>
#include "User.h"
#include "SecondaryStat.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"

#define REGISTER_TS(name, value)\
retTSFlag |= GET_TS_FLAG(##name);\
pRef = &pSS->m_mSetByTS[TemporaryStat::TS_##name]; pRef->second.clear();\
pSS->n##name = bResetByItem ? 0 : value;\
pSS->r##name = bResetByItem ? 0 : -nItemID;\
pSS->t##name = bResetByItem ? 0 : tTime;\
pSS->nLv##name = 0;\
if(!bResetByItem)\
{\
	pRef->first = bForcedSetTime ? nForcedSetTime : tCur;\
	pRef->second.push_back(&pSS->n##name);\
	pRef->second.push_back(&pSS->r##name);\
	pRef->second.push_back(&pSS->t##name);\
	pRef->second.push_back(&pSS->nLv##name);\
}\

TemporaryStat::TS_Flag StateChangeItem::Apply(User * pUser, int tCur, bool bApplyBetterOnly, bool bResetByItem, bool bForcedSetTime, int nForcedSetTime)
{
	auto pSS = pUser->GetSecondaryStat();
	auto pBS = pUser->GetBasicStat();
	auto pS = pUser->GetCharacterData()->mStat;
	auto info = spec.end(), end = spec.end(), time = spec.find("time");

	std::pair<long long int, std::vector<int*>>* pRef = nullptr;
	long long int liFlag = 0;
	int tTime = (time == end ? 0 : time->second);
	TemporaryStat::TS_Flag retTSFlag;

	for (auto& info : spec)
	{
		std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
		if (info.first == "hp")
		{
			pS->nHP += info.second;
			pS->nHP = pS->nHP > pBS->nMHP ? pBS->nMHP : pS->nHP;
			liFlag |= BasicStat::BS_HP;
		}
		else if (info.first == "mp")
		{
			pS->nMP += info.second;
			pS->nMP = pS->nMP > pBS->nMMP ? pBS->nMMP : pS->nMP;
			liFlag |= BasicStat::BS_MP;
		}
		if (info.first == "hpR")
		{
			pS->nHP += pBS->nMHP * (info.second) / 100;
			pS->nHP = pS->nHP > pBS->nMHP ? pBS->nMHP : pS->nHP;
			liFlag |= BasicStat::BS_HP;
		}
		else if (info.first == "mpR")
		{
			pS->nMP += pBS->nMMP * (info.second) / 100;
			pS->nMP = pS->nMP > pBS->nMMP ? pBS->nMMP : pS->nMP;
			liFlag |= BasicStat::BS_MP;
		}
		else if (info.first == "mad")
		{
			REGISTER_TS(MAD, info.second);
		}
		else if (info.first == "mdd")
		{
			REGISTER_TS(MDD, info.second);
		}
		else if (info.first == "pad")
		{
			REGISTER_TS(PAD, info.second);
		}
		else if (info.first == "pdd")
		{
			REGISTER_TS(PDD, info.second);
		}
		else if (info.first == "acc")
		{
			REGISTER_TS(ACC, info.second);
		}
		else if (info.first == "eva")
		{
			REGISTER_TS(EVA, info.second);
		}
		else if (info.first == "craft")
		{
			REGISTER_TS(Craft, info.second);
		}
		else if (info.first == "speed")
		{
			REGISTER_TS(Speed, info.second);
		}
		else if (info.first == "jump")
		{
			REGISTER_TS(Jump, info.second);
		}
		else if (info.first == "thaw")
		{
			REGISTER_TS(Thaw, info.second);
		}
		else if (info.first == "morph")
		{
			REGISTER_TS(Morph, info.second);
		}
		else if (info.first == "mesoupbyitem")
		{
			REGISTER_TS(MesoUpByItem, info.second);
		}
		else if (info.first == "weakness")
		{
			REGISTER_TS(Weakness, info.second);
		}
		else if (info.first == "darkness")
		{
			REGISTER_TS(Darkness, info.second);
		}
		else if (info.first == "curse")
		{
			REGISTER_TS(Curse, info.second);
		}
		else if (info.first == "seal")
		{
			REGISTER_TS(Seal, info.second);
		}
		else if (info.first == "poison")
		{
			REGISTER_TS(Poison, info.second);
		}
		else if (info.first == "stun")
		{
			REGISTER_TS(Stun, info.second);
		}
		else if (info.first == "slow")
		{
			REGISTER_TS(Slow, info.second);
		}
		else if (info.first == "mhp_temp")
		{
			REGISTER_TS(MaxHP, info.second);
		}
		else if (info.first == "mmp_temp")
		{
			REGISTER_TS(MaxMP, info.second);
		}
	}

	pUser->SendCharacterStat(true, liFlag);
	pUser->ValidateStat();

	return retTSFlag;
}