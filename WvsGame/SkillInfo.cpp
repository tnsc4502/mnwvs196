
#include "..\WvsLib\Evaluator\Evaluator.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_SkillRecord.h"
#include "ItemInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "SkillInfo.h"

#include <thread>
#include <unordered_map>


#define CHECK_SKILL_ATTRIBUTE(var, attribute) if(attributeSet.find(#attribute) != attributeSetEnd) (mappingTable[(&(var)) - pAttributeBase]=(std::string)skillCommonImg[#attribute]);
#define PARSE_SKILLDATA(attribute) ((std::string)skillCommonImg[#attribute]) == "" ? 0 : (int)Evaluator::Eval(((std::string)skillCommonImg[#attribute]), "", d);

SkillInfo::SkillInfo()
{
	m_nOnLoadingSkills = 0;
}


SkillInfo::~SkillInfo()
{
}

int SkillInfo::GetLoadingSkillCount() const
{
	return m_nOnLoadingSkills;
}

const std::map<int, std::map<int, SkillEntry*>*>& SkillInfo::GetSkills() const
{
	return m_mSkillByRootID;
}

const std::map<int, SkillEntry*>* SkillInfo::GetSkillsByRootID(int nRootID) const
{
	return m_mSkillByRootID.at(nRootID);
}

const SkillEntry * SkillInfo::GetSkillByID(int nSkillID) const
{
	int nJobID = nSkillID / 10000;
	auto rootData = GetSkillsByRootID(nJobID);
	auto findResult = rootData->find(nSkillID);
	return (findResult != rootData->end() ? findResult->second : nullptr);
}

SkillInfo * SkillInfo::GetInstance()
{
	static SkillInfo* pInstance = new SkillInfo;
	return pInstance;
}

int SkillInfo::GetBundleItemMaxPerSlot(int nItemID, GA_Character * pCharacterData)
{
	auto pItem = ItemInfo::GetInstance()->GetBundleItem(nItemID);
	if (pItem != nullptr)
	{
		int result = pItem->nMaxPerSlot;
		if (pCharacterData != nullptr &&  nItemID / 10000 == 207)
		{
			//精準暗器
		}
		return result == 0 ? 100 : result;
	}
	return 0;
}

void SkillInfo::IterateSkillInfo()
{
	WvsLogger::LogRaw("[SkillInfo::IterateSkillInfo]開始載入所有技能資訊 IterateSkillInfo Start.\n");
	static auto& skillWz = stWzResMan->GetWz(Wz::Skill);
	bool continued = false;
	int nRootID;
	for (auto& node : skillWz)
	{
		continued = false;
		auto& str = node.Name();
		for(char c : str)
			if (!isdigit(c))
			{
				continued = true;
				break;
			}
		if (continued)
			continue;
		nRootID = atoi(str.c_str());
		//LoadSkillRoot(nRootID, (void*)(&node["skill"]));
		std::thread t(&SkillInfo::LoadSkillRoot, this, nRootID, (void*)(&node["skill"]));
		t.detach();
	}
	//printf("[SkillInfo::IterateSkillInfo]技能資訊載入完畢 IterateSkillInfo End.\n");
}

void SkillInfo::LoadSkillRoot(int nSkillRootID, void * pData)
{
	auto skillRootIter = m_mSkillByRootID.find(nSkillRootID);
	if (skillRootIter == m_mSkillByRootID.end()) 
		m_mSkillByRootID.insert({nSkillRootID, new std::map<int, SkillEntry*>() });
	auto& skillRootImg = *((WZ::Node*)pData);
	int nSkillID = 0;
	for (auto& skillImg : skillRootImg)
	{
		++m_nOnLoadingSkills;
		nSkillID = atoi(skillImg.Name().c_str());
		LoadSkill(nSkillRootID, nSkillID, (void*)&skillImg);
		//std::thread t(&SkillInfo::LoadSkill, this, nSkillRootID, nSkillID, (void*)&skillImg);
		//t.detach();
		--m_nOnLoadingSkills;
	}
	if (m_nOnLoadingSkills == 0 && m_mSkillByRootID.size() >= 221)
	{
		WvsLogger::LogRaw("[SkillInfo::IterateSkillInfo]技能資訊載入完畢 IterateSkillInfo End.\n");
		stWzResMan->ReleaseMemory();
		//system("pause");
		//auto pSkill = GetSkillByID(2211010)->GetLevelData(5);
		//printf("[SkillInfo::IterateSkillInfo]技能資訊載入完畢 IterateSkillInfo End 2 %d.\n", pSkill->m_nMpCon);
		/*for (auto& p : m_mSkillByRootID)
		{
			for (auto& pp : *(p.second))
			{
				//if(!pp.second->GetLevelData(1))
					printf("ID: %d Total : %d\n", pp.second->GetSkillID(), (int)pp.second->GetAllLevelData().size());
			}
		}*/
		/*int nTest = GetSkillByID(1000)->GetMaxLevel();
		printf("Test 2211010 : %d %d %d %d %d %d\n", 
			nTest,
			GetSkillByID(1000)->GetLevelData(3)->m_nMpCon,
			GetSkillByID(1000)->GetLevelData(3)->m_nDamage,
			GetSkillByID(1000)->GetLevelData(3)->m_nMobCount,
			GetSkillByID(1000)->GetLevelData(3)->m_nAttackCount,
			GetSkillByID(1000)->GetLevelData(3)->m_nTime);*/
	}
}

SkillEntry * SkillInfo::LoadSkill(int nSkillRootID, int nSkillID, void * pData)
{
	bool bLevelStructure = false;
	auto& skillDataImg = *((WZ::Node*)pData);
	auto& skillCommonImg = skillDataImg["common"];
	if (((nSkillRootID >= 800000 || nSkillRootID % 1000 < 10 || (nSkillRootID >= 9000 && nSkillRootID <= 9500))
		&& (((int)skillCommonImg["maxLevel"] == 0)))) //部分初心者技能
	{
		bLevelStructure = true;
		skillCommonImg = skillDataImg["level"];
	}
	auto& skillInfoImg = skillDataImg["info"];
	auto& skillListImg = skillDataImg["skillList"];
	auto& skillReqImg = skillDataImg["req"];
	SkillEntry* pResult = new SkillEntry;
	pResult->SetSkillID(nSkillID);
	pResult->SetMasterLevel(atoi(((std::string)skillDataImg["masterLevel"]).c_str()));
	pResult->SetMaxLevel(atoi(((std::string)skillCommonImg["maxLevel"]).c_str()));
	if(bLevelStructure)
		LoadLevelDataByLevelNode(nSkillID, pResult, (void*)&skillCommonImg);
	else
		LoadLevelData(nSkillID, pResult, (void*)&skillCommonImg);
	
	std::lock_guard<std::mutex> lock(m_mtxSkillResLock);
	m_mSkillByRootID[nSkillRootID]->insert({ nSkillID, pResult });
	return pResult;
}

void SkillInfo::LoadLevelData(int nSkillID, SkillEntry * pEntry, void * pData)
{
	auto& skillCommonImg = *((WZ::Node*)pData);
	int nMaxLevel = pEntry->GetMaxLevel();
	double d;
	pEntry->AddLevelData(nullptr); //lvl 0

	//變數對應到的算式，key是變數的指標位置與 m_nLevelDataPtrBase 的距離
	std::map<std::ptrdiff_t, std::string> mappingTable;

	//屬性對應到的算式
	std::unordered_map<std::string, std::string> attributeSet;
	for (auto& skill : skillCommonImg)
		attributeSet[skill.Name()] = (std::string)skill;

	//避免.end()一直被呼叫
	auto attributeSetEnd = attributeSet.end();

	//for (int i = 1; i <= 1; ++i)
	//{
		SkillLevelData* pLevelData = new SkillLevelData();

		//找出Level Data的位置基底
		int* pAttributeBase = (&(pLevelData->m_nLevelDataPtrBase)); //for point ref.

		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndiePad, indiePad);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieMad, indieMad);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndiePdd, indiePdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieMdd, indieMdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieMhp, indieMhp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieMhpR, indieMhpR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieMmp, indieMmp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieMmpR, indieMmpR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieAcc, indieAcc);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieEva, indieEva);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieJump, indieJump);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieSpeed, indieSpeed);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieAllStat, indieAllStat);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieBooster, indieBooster);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieDamR, indieDamR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieMaxDamageOver, indieMaxDamageOver);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieAsrR, indieAsrR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieTerR, indieTerR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieCr, indieCr);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieBDR, indieBDR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieStance, indieStance);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndiePadR, indiePadR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPVPdamage, PVPdamage);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nAbnormalDamR, abnormalDamR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nAcc, acc);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nAcc2dam, acc2dam);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nAcc2mp, acc2mp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nAccR, accR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nAccX, accX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nAr, ar);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nAsrR, asrR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nAttackCount, attackCount);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDamR_5th, damR_5th);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nBdR, bdR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nBufftimeR, bufftimeR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nBulletConsume, bulletConsume);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nBulletCount, bulletCount);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nCoolTimeR, coolTimeR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nCooltime, cooltime);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nCr, cr);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nActionSpeed, actionSpeed);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nCriticaldamageMax, criticaldamageMax);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nCriticaldamageMin, criticaldamageMin);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDamR, damR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDamage, damage);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDamagepc, damagepc);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDateExpire, dateExpire);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDex, dex);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDex2str, dex2str);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDexFX, dexFX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDexX, dexX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDot, dot);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDotInterval, dotInterval);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDotSuperpos, dotSuperpos);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDotTime, dotTime);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nDropR, dropR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEmad, emad);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEmdd, emdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEmhp, emhp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEmmp, emmp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEpad, epad);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEpdd, epdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEr, er);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEva, eva);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEva2hp, eva2hp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEvaR, evaR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nEvaX, evaX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nExpLossReduceR, expLossReduceR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nExpR, expR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nExtendPrice, extendPrice);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nFinalAttackDamR, finalAttackDamR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nFixdamage, fixdamage);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nForceCon, forceCon);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMDF, MDF);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPowerCon, powerCon);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nHp, hp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nHpCon, hpCon);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIceGageCon, iceGageCon);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIgnoreMobDamR, ignoreMobDamR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIgnoreMobpdpR, ignoreMobpdpR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nInt2luk, int2luk);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIntFX, intFX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIntX, intX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nItemCon, itemCon);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nItemConNo, itemConNo);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nItemConsume, itemConsume);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nJump, jump);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nKp, kp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLuk2dex, luk2dex);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLukFX, lukFX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLukX, lukX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLv2damX, lv2damX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLv2mad, lv2mad);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLv2mdX, lv2mdX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLv2pad, lv2pad);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLv2pdX, lv2pdX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMad, mad);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMadX, madX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMastery, mastery);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMdd, mdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMdd2dam, mdd2dam);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMdd2pdd, mdd2pdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMdd2pdx, mdd2pdx);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMddR, mddR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMddX, mddX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMesoR, mesoR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMhp2damX, mhp2damX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMhpR, mhpR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMhpX, mhpX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMinionDeathProp, minionDeathProp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMmp2damX, mmp2damX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMmpR, mmpR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMmpX, mmpX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nOnActive, onActive);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMobCount, mobCount);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMorph, morph);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMp, mp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMpCon, mpCon);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMpConEff, mpConEff);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMpConReduce, mpConReduce);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nNbdR, nbdR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nNocoolProp, nocoolProp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nOnHitHpRecoveryR, onHitHpRecoveryR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nOnHitMpRecoveryR, onHitMpRecoveryR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPad, pad);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPadX, padX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPassivePlus, passivePlus);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPdd, pdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPdd2dam, pdd2dam);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPdd2mdd, pdd2mdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPdd2mdx, pdd2mdx);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPddR, pddR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPddX, pddX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPeriod, period);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPpCon, ppCon);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPpRecovery, ppRecovery);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPrice, price);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPriceUnit, priceUnit);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nProp, prop);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPsdJump, psdJump);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPsdSpeed, psdSpeed);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nRange, range);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nReqGuildLevel, reqGuildLevel);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nSelfDestruction, selfDestruction);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nSpeed, speed);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nSpeedMax, speedMax);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nStr, str);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nStr2dex, str2dex);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nStrFX, strFX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nStrX, strX);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nSubProp, subProp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nSubTime, subTime);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nSuddenDeathR, suddenDeathR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nSummonTimeR, summonTimeR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nTargetPlus, targetPlus);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nTdR, tdR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nTerR, terR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nTime, time);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nS, s);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nT, t);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nU, u);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nV, v);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nW, w);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nX, x);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nY, y);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nZ, z);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nInt, int);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLuk, luk);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nHpR, hpR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMpR, mpR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIgnoreMobpdpR, ignoreMob);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nThaw, thaw);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nInterval, interval);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nExpInc, expinc);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nExp, exp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMorphID, morphId);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nCP, cp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nCosmetic, cosmetic);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nSlotCount, slotCount);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nPreventSlip, preventslip);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nUseLevel, useLevel);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nNuffSkill, nuffSkill);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nFamiliarTarget, familiarTarget);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nImmortal, immortal);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nType, type);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nBS, bs);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndiePdd, indiePdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndieMdd, indieMdd);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIndiePadR, indiePadR);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nExpBuff, expBuff);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nCashUp, cashup);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nItemUp, itemup);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nMesoUp, mesoup);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nBerserk, berserk);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nBerserk2, berserk2);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nBooster, booster);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nLifeID, lifeId);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nInflation, inflation);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nImHp, imhp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nImMp, immp);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_nIllusion, illusion);
		CHECK_SKILL_ATTRIBUTE(pLevelData->m_bConsumeOnPickup, consumeOnPickup);

		//下面這個先暫時忽略
		/*if (pLevelData->m_bConsumeOnPickup)
		{
			int party = 0;
			//party = PARSE_SKILLDATA2(party);
			pLevelData->m_bConsumeOnPickup = party > 0 ? 2 : pLevelData->m_bConsumeOnPickup;
		}*/
		delete pLevelData;
	//}

	SkillLevelData** apLevelData = new SkillLevelData*[nMaxLevel];
	for (int i = 0; i < nMaxLevel; ++i)
	{
		apLevelData[i] = new SkillLevelData;
		pEntry->AddLevelData(apLevelData[i]);
	}
	for (auto &p : mappingTable)
	{
		Evaluator eval(d, p.second);
		for (int i = 1; i <= nMaxLevel; ++i)
		{
			d = i;
			SkillLevelData* pLevelData = apLevelData[i - 1];
			*(((int*)&(pLevelData->m_nLevelDataPtrBase)) + p.first) = (int)eval.Eval();
		}
	}
	delete[] apLevelData;
}

void SkillInfo::LoadLevelDataByLevelNode(int nSkillID, SkillEntry * pEntry, void * pData)
{
	auto& skillLevelImg = *((WZ::Node*)pData);
	int nMaxLevel = pEntry->GetMaxLevel();
	pEntry->AddLevelData(nullptr); //for lvl 0

	double d = 0;
	for (auto& skillCommonImg : skillLevelImg)
	{
		d = atof(skillCommonImg.Name().c_str());

		SkillLevelData* pLevelData = new SkillLevelData;
		pLevelData->m_nIndiePad = PARSE_SKILLDATA(indiePad);
		pLevelData->m_nIndieMad = PARSE_SKILLDATA(indieMad);
		pLevelData->m_nIndiePdd = PARSE_SKILLDATA(indiePdd);
		pLevelData->m_nIndieMdd = PARSE_SKILLDATA(indieMdd);
		pLevelData->m_nIndieMhp = PARSE_SKILLDATA(indieMhp);
		pLevelData->m_nIndieMhpR = PARSE_SKILLDATA(indieMhpR);
		pLevelData->m_nIndieMmp = PARSE_SKILLDATA(indieMmp);
		pLevelData->m_nIndieMmpR = PARSE_SKILLDATA(indieMmpR);
		pLevelData->m_nIndieAcc = PARSE_SKILLDATA(indieAcc);
		pLevelData->m_nIndieEva = PARSE_SKILLDATA(indieEva);
		pLevelData->m_nIndieJump = PARSE_SKILLDATA(indieJump);
		pLevelData->m_nIndieSpeed = PARSE_SKILLDATA(indieSpeed);
		pLevelData->m_nIndieAllStat = PARSE_SKILLDATA(indieAllStat);
		pLevelData->m_nIndieBooster = PARSE_SKILLDATA(indieBooster);
		pLevelData->m_nIndieDamR = PARSE_SKILLDATA(indieDamR);
		pLevelData->m_nIndieMaxDamageOver = PARSE_SKILLDATA(indieMaxDamageOver);
		pLevelData->m_nIndieAsrR = PARSE_SKILLDATA(indieAsrR);
		pLevelData->m_nIndieTerR = PARSE_SKILLDATA(indieTerR);
		pLevelData->m_nIndieCr = PARSE_SKILLDATA(indieCr);
		pLevelData->m_nIndieBDR = PARSE_SKILLDATA(indieBDR);
		pLevelData->m_nIndieStance = PARSE_SKILLDATA(indieStance);
		pLevelData->m_nIndiePadR = PARSE_SKILLDATA(indiePadR);
		pLevelData->m_nPVPdamage = PARSE_SKILLDATA(PVPdamage);
		pLevelData->m_nAbnormalDamR = PARSE_SKILLDATA(abnormalDamR);
		pLevelData->m_nAcc = PARSE_SKILLDATA(acc);
		pLevelData->m_nAcc2dam = PARSE_SKILLDATA(acc2dam);
		pLevelData->m_nAcc2mp = PARSE_SKILLDATA(acc2mp);
		pLevelData->m_nAccR = PARSE_SKILLDATA(accR);
		pLevelData->m_nAccX = PARSE_SKILLDATA(accX);
		pLevelData->m_nAr = PARSE_SKILLDATA(ar);
		pLevelData->m_nAsrR = PARSE_SKILLDATA(asrR);
		pLevelData->m_nAttackCount = PARSE_SKILLDATA(attackCount);
		pLevelData->m_nDamR_5th = PARSE_SKILLDATA(damR_5th);
		pLevelData->m_nBdR = PARSE_SKILLDATA(bdR);
		pLevelData->m_nBufftimeR = PARSE_SKILLDATA(bufftimeR);
		pLevelData->m_nBulletConsume = PARSE_SKILLDATA(bulletConsume);
		pLevelData->m_nBulletCount = PARSE_SKILLDATA(bulletCount);
		pLevelData->m_nCoolTimeR = PARSE_SKILLDATA(coolTimeR);
		pLevelData->m_nCooltime = PARSE_SKILLDATA(cooltime);
		pLevelData->m_nCr = PARSE_SKILLDATA(cr);
		pLevelData->m_nActionSpeed = PARSE_SKILLDATA(actionSpeed);
		pLevelData->m_nCriticaldamageMax = PARSE_SKILLDATA(criticaldamageMax);
		pLevelData->m_nCriticaldamageMin = PARSE_SKILLDATA(criticaldamageMin);
		pLevelData->m_nDamR = PARSE_SKILLDATA(damR);
		pLevelData->m_nDamage = PARSE_SKILLDATA(damage);
		pLevelData->m_nDamagepc = PARSE_SKILLDATA(damagepc);
		pLevelData->m_nDateExpire = PARSE_SKILLDATA(dateExpire);
		pLevelData->m_nDex = PARSE_SKILLDATA(dex);
		pLevelData->m_nDex2str = PARSE_SKILLDATA(dex2str);
		pLevelData->m_nDexFX = PARSE_SKILLDATA(dexFX);
		pLevelData->m_nDexX = PARSE_SKILLDATA(dexX);
		pLevelData->m_nDot = PARSE_SKILLDATA(dot);
		pLevelData->m_nDotInterval = PARSE_SKILLDATA(dotInterval);
		pLevelData->m_nDotSuperpos = PARSE_SKILLDATA(dotSuperpos);
		pLevelData->m_nDotTime = PARSE_SKILLDATA(dotTime);
		pLevelData->m_nDropR = PARSE_SKILLDATA(dropR);
		pLevelData->m_nEmad = PARSE_SKILLDATA(emad);
		pLevelData->m_nEmdd = PARSE_SKILLDATA(emdd);
		pLevelData->m_nEmhp = PARSE_SKILLDATA(emhp);
		pLevelData->m_nEmmp = PARSE_SKILLDATA(emmp);
		pLevelData->m_nEpad = PARSE_SKILLDATA(epad);
		pLevelData->m_nEpdd = PARSE_SKILLDATA(epdd);
		pLevelData->m_nEr = PARSE_SKILLDATA(er);
		pLevelData->m_nEva = PARSE_SKILLDATA(eva);
		pLevelData->m_nEva2hp = PARSE_SKILLDATA(eva2hp);
		pLevelData->m_nEvaR = PARSE_SKILLDATA(evaR);
		pLevelData->m_nEvaX = PARSE_SKILLDATA(evaX);
		pLevelData->m_nExpLossReduceR = PARSE_SKILLDATA(expLossReduceR);
		pLevelData->m_nExpR = PARSE_SKILLDATA(expR);
		pLevelData->m_nExtendPrice = PARSE_SKILLDATA(extendPrice);
		pLevelData->m_nFinalAttackDamR = PARSE_SKILLDATA(finalAttackDamR);
		pLevelData->m_nFixdamage = PARSE_SKILLDATA(fixdamage);
		pLevelData->m_nForceCon = PARSE_SKILLDATA(forceCon);
		pLevelData->m_nMDF = PARSE_SKILLDATA(MDF);
		pLevelData->m_nPowerCon = PARSE_SKILLDATA(powerCon);
		pLevelData->m_nHp = PARSE_SKILLDATA(hp);
		pLevelData->m_nHpCon = PARSE_SKILLDATA(hpCon);
		pLevelData->m_nIceGageCon = PARSE_SKILLDATA(iceGageCon);
		pLevelData->m_nIgnoreMobDamR = PARSE_SKILLDATA(ignoreMobDamR);
		pLevelData->m_nIgnoreMobpdpR = PARSE_SKILLDATA(ignoreMobpdpR);
		pLevelData->m_nInt2luk = PARSE_SKILLDATA(int2luk);
		pLevelData->m_nIntFX = PARSE_SKILLDATA(intFX);
		pLevelData->m_nIntX = PARSE_SKILLDATA(intX);
		pLevelData->m_nItemCon = PARSE_SKILLDATA(itemCon);
		pLevelData->m_nItemConNo = PARSE_SKILLDATA(itemConNo);
		pLevelData->m_nItemConsume = PARSE_SKILLDATA(itemConsume);
		pLevelData->m_nJump = PARSE_SKILLDATA(jump);
		pLevelData->m_nKp = PARSE_SKILLDATA(kp);
		pLevelData->m_nLuk2dex = PARSE_SKILLDATA(luk2dex);
		pLevelData->m_nLukFX = PARSE_SKILLDATA(lukFX);
		pLevelData->m_nLukX = PARSE_SKILLDATA(lukX);
		pLevelData->m_nLv2damX = PARSE_SKILLDATA(lv2damX);
		pLevelData->m_nLv2mad = PARSE_SKILLDATA(lv2mad);
		pLevelData->m_nLv2mdX = PARSE_SKILLDATA(lv2mdX);
		pLevelData->m_nLv2pad = PARSE_SKILLDATA(lv2pad);
		pLevelData->m_nLv2pdX = PARSE_SKILLDATA(lv2pdX);
		pLevelData->m_nMad = PARSE_SKILLDATA(mad);
		pLevelData->m_nMadX = PARSE_SKILLDATA(madX);
		pLevelData->m_nMastery = PARSE_SKILLDATA(mastery);
		pLevelData->m_nMdd = PARSE_SKILLDATA(mdd);
		pLevelData->m_nMdd2dam = PARSE_SKILLDATA(mdd2dam);
		pLevelData->m_nMdd2pdd = PARSE_SKILLDATA(mdd2pdd);
		pLevelData->m_nMdd2pdx = PARSE_SKILLDATA(mdd2pdx);
		pLevelData->m_nMddR = PARSE_SKILLDATA(mddR);
		pLevelData->m_nMddX = PARSE_SKILLDATA(mddX);
		pLevelData->m_nMesoR = PARSE_SKILLDATA(mesoR);
		pLevelData->m_nMhp2damX = PARSE_SKILLDATA(mhp2damX);
		pLevelData->m_nMhpR = PARSE_SKILLDATA(mhpR);
		pLevelData->m_nMhpX = PARSE_SKILLDATA(mhpX);
		pLevelData->m_nMinionDeathProp = PARSE_SKILLDATA(minionDeathProp);
		pLevelData->m_nMmp2damX = PARSE_SKILLDATA(mmp2damX);
		pLevelData->m_nMmpR = PARSE_SKILLDATA(mmpR);
		pLevelData->m_nMmpX = PARSE_SKILLDATA(mmpX);
		pLevelData->m_nOnActive = PARSE_SKILLDATA(onActive);
		pLevelData->m_nMobCount = PARSE_SKILLDATA(mobCount);
		pLevelData->m_nMorph = PARSE_SKILLDATA(morph);
		pLevelData->m_nMp = PARSE_SKILLDATA(mp);
		pLevelData->m_nMpCon = PARSE_SKILLDATA(mpCon);
		pLevelData->m_nMpConEff = PARSE_SKILLDATA(mpConEff);
		pLevelData->m_nMpConReduce = PARSE_SKILLDATA(mpConReduce);
		pLevelData->m_nNbdR = PARSE_SKILLDATA(nbdR);
		pLevelData->m_nNocoolProp = PARSE_SKILLDATA(nocoolProp);
		pLevelData->m_nOnHitHpRecoveryR = PARSE_SKILLDATA(onHitHpRecoveryR);
		pLevelData->m_nOnHitMpRecoveryR = PARSE_SKILLDATA(onHitMpRecoveryR);
		pLevelData->m_nPad = PARSE_SKILLDATA(pad);
		pLevelData->m_nPadX = PARSE_SKILLDATA(padX);
		pLevelData->m_nPassivePlus = PARSE_SKILLDATA(passivePlus);
		pLevelData->m_nPdd = PARSE_SKILLDATA(pdd);
		pLevelData->m_nPdd2dam = PARSE_SKILLDATA(pdd2dam);
		pLevelData->m_nPdd2mdd = PARSE_SKILLDATA(pdd2mdd);
		pLevelData->m_nPdd2mdx = PARSE_SKILLDATA(pdd2mdx);
		pLevelData->m_nPddR = PARSE_SKILLDATA(pddR);
		pLevelData->m_nPddX = PARSE_SKILLDATA(pddX);
		pLevelData->m_nPeriod = PARSE_SKILLDATA(period);
		pLevelData->m_nPpCon = PARSE_SKILLDATA(ppCon);
		pLevelData->m_nPpRecovery = PARSE_SKILLDATA(ppRecovery);
		pLevelData->m_nPrice = PARSE_SKILLDATA(price);
		pLevelData->m_nPriceUnit = PARSE_SKILLDATA(priceUnit);
		pLevelData->m_nProp = PARSE_SKILLDATA(prop);
		pLevelData->m_nPsdJump = PARSE_SKILLDATA(psdJump);
		pLevelData->m_nPsdSpeed = PARSE_SKILLDATA(psdSpeed);
		pLevelData->m_nRange = PARSE_SKILLDATA(range);
		pLevelData->m_nReqGuildLevel = PARSE_SKILLDATA(reqGuildLevel);
		pLevelData->m_nSelfDestruction = PARSE_SKILLDATA(selfDestruction);
		pLevelData->m_nSpeed = PARSE_SKILLDATA(speed);
		pLevelData->m_nSpeedMax = PARSE_SKILLDATA(speedMax);
		pLevelData->m_nStr = PARSE_SKILLDATA(str);
		pLevelData->m_nStr2dex = PARSE_SKILLDATA(str2dex);
		pLevelData->m_nStrFX = PARSE_SKILLDATA(strFX);
		pLevelData->m_nStrX = PARSE_SKILLDATA(strX);
		pLevelData->m_nSubProp = PARSE_SKILLDATA(subProp);
		pLevelData->m_nSubTime = PARSE_SKILLDATA(subTime);
		pLevelData->m_nSuddenDeathR = PARSE_SKILLDATA(suddenDeathR);
		pLevelData->m_nSummonTimeR = PARSE_SKILLDATA(summonTimeR);
		pLevelData->m_nTargetPlus = PARSE_SKILLDATA(targetPlus);
		pLevelData->m_nTdR = PARSE_SKILLDATA(tdR);
		pLevelData->m_nTerR = PARSE_SKILLDATA(terR);
		pLevelData->m_nTime = PARSE_SKILLDATA(time);
		pLevelData->m_nS = PARSE_SKILLDATA(s);
		pLevelData->m_nT = PARSE_SKILLDATA(t);
		pLevelData->m_nU = PARSE_SKILLDATA(u);
		pLevelData->m_nV = PARSE_SKILLDATA(v);
		pLevelData->m_nW = PARSE_SKILLDATA(w);
		pLevelData->m_nX = PARSE_SKILLDATA(x);
		pLevelData->m_nY = PARSE_SKILLDATA(y);
		pLevelData->m_nZ = PARSE_SKILLDATA(z);
		pLevelData->m_nInt = PARSE_SKILLDATA(int);
		pLevelData->m_nLuk = PARSE_SKILLDATA(luk);

		pLevelData->m_nHpR = PARSE_SKILLDATA(hpR);
		pLevelData->m_nMpR = PARSE_SKILLDATA(mpR);
		pLevelData->m_nIgnoreMobpdpR = PARSE_SKILLDATA(ignoreMob);
		pLevelData->m_nThaw = PARSE_SKILLDATA(thaw);
		pLevelData->m_nInterval = PARSE_SKILLDATA(interval);
		pLevelData->m_nExpInc = PARSE_SKILLDATA(expinc);
		pLevelData->m_nExp = PARSE_SKILLDATA(exp);
		pLevelData->m_nMorphID = PARSE_SKILLDATA(morphId);
		pLevelData->m_nCP = PARSE_SKILLDATA(cp);
		pLevelData->m_nCosmetic = PARSE_SKILLDATA(cosmetic);
		pLevelData->m_nSlotCount = PARSE_SKILLDATA(slotCount);
		pLevelData->m_nPreventSlip = PARSE_SKILLDATA(preventslip);
		pLevelData->m_nUseLevel = PARSE_SKILLDATA(useLevel);
		pLevelData->m_nNuffSkill = PARSE_SKILLDATA(nuffSkill);
		pLevelData->m_nFamiliarTarget = PARSE_SKILLDATA(familiarTarget);
		pLevelData->m_nImmortal = PARSE_SKILLDATA(immortal);
		pLevelData->m_nType = PARSE_SKILLDATA(type);
		pLevelData->m_nBS = PARSE_SKILLDATA(bs);
		pLevelData->m_nIndiePdd = PARSE_SKILLDATA(indiePdd);
		pLevelData->m_nIndieMdd = PARSE_SKILLDATA(indieMdd);
		pLevelData->m_nIndiePadR = PARSE_SKILLDATA(indiePadR);
		pLevelData->m_nExpBuff = PARSE_SKILLDATA(expBuff);
		pLevelData->m_nCashUp = PARSE_SKILLDATA(cashup);
		pLevelData->m_nItemUp = PARSE_SKILLDATA(itemup);
		pLevelData->m_nMesoUp = PARSE_SKILLDATA(mesoup);
		pLevelData->m_nBerserk = PARSE_SKILLDATA(berserk);
		pLevelData->m_nBerserk2 = PARSE_SKILLDATA(berserk2);
		pLevelData->m_nBooster = PARSE_SKILLDATA(booster);
		pLevelData->m_nLifeID = PARSE_SKILLDATA(lifeId);
		pLevelData->m_nInflation = PARSE_SKILLDATA(inflation);
		pLevelData->m_nImHp = PARSE_SKILLDATA(imhp);
		pLevelData->m_nImMp = PARSE_SKILLDATA(immp);
		pLevelData->m_nIllusion = PARSE_SKILLDATA(illusion);
		pLevelData->m_bConsumeOnPickup = PARSE_SKILLDATA(consumeOnPickup);
		if (pLevelData->m_bConsumeOnPickup)
		{
			int party = 0;
			party = PARSE_SKILLDATA(party);
			pLevelData->m_bConsumeOnPickup = party > 0 ? 2 : pLevelData->m_bConsumeOnPickup;
		}

		pEntry->AddLevelData(pLevelData);
	}
	pEntry->SetMaxLevel((int)pEntry->GetAllLevelData().size());
}

/*void SkillInfo::LoadLevelDataSpecial()
{
	//what the.... what happened on those skills...
	int special[] = { 2211010,
	   20040218,
	   20040221,
	   27111005,
	   32120001 };

	for (auto i : special) 
	{
		delete m_mSkillByRootID[i / 10000]->at(i);
		m_mSkillByRootID[i / 10000]->erase(i);
	}

	////////////////////////////////2211010//////////////////////////////
	SkillEntry* pEntry1 = new SkillEntry;
	pEntry1->SetSkillID(2211010);
	pEntry1->SetMasterLevel(20);
	pEntry1->SetMaxLevel(20);
	pEntry1->AddLevelData(nullptr);	
	//exprtk::parser<double> parser;
	double d = 0;
	Evaluator evaluator; 
	for (int i = 1; i <= pEntry1->GetMaxLevel(); ++i)
	{
		d = i;
		SkillLevelData* pLevelData = new SkillLevelData;
		pLevelData->m_nMpCon = /*40+5*ceil(i/4)*//* PARSE_SKILLDATA_STRING(40+5*u(x/4));
		/*pLevelData->m_nDamage = 243+7*i;
		pLevelData->m_nMobCount = 12;
		pLevelData->m_nAttackCount = 3;
		pLevelData->m_nTime = 8;
		pLevelData->m_nLt = -90;
		pLevelData->m_nRb = 15;
		pLevelData->m_nS = -15;
		pLevelData->m_nV = -75;
		pEntry1->AddLevelData(pLevelData);
	}
	m_mSkillByRootID[221]->insert({ pEntry1->GetSkillID(),  pEntry1 });
	////////////////////////////////2211010//////////////////////////////

	////////////////////////////////20040218//////////////////////////////
	pEntry1 = new SkillEntry;
	pEntry1->SetSkillID(20040218);
	pEntry1->SetMasterLevel(2);
	pEntry1->SetMaxLevel(2);
	pEntry1->AddLevelData(nullptr);
	for (int i = 1; i <= pEntry1->GetMaxLevel(); ++i)
	{
		d = i;

		SkillLevelData* pLevelData = new SkillLevelData;
		pLevelData->m_nIgnoreMobpdpR = /*40+5*ceil(i/4)*/ /*PARSE_SKILLDATA_STRING(5 + 5 * x);

		pEntry1->AddLevelData(pLevelData);
	}
	m_mSkillByRootID[2004]->insert({ pEntry1->GetSkillID(),  pEntry1 });
	////////////////////////////////20040218//////////////////////////////

	////////////////////////////////20040221//////////////////////////////
	pEntry1 = new SkillEntry;
	pEntry1->SetSkillID(20040221);
	pEntry1->SetMasterLevel(1);
	pEntry1->SetMaxLevel(1);
	pEntry1->AddLevelData(nullptr);
	for (int i = 1; i <= pEntry1->GetMaxLevel(); ++i)
	{
		d = i;

		SkillLevelData* pLevelData = new SkillLevelData;
		pLevelData->m_nIntX = 20;

		pEntry1->AddLevelData(pLevelData);
	}
	m_mSkillByRootID[2004]->insert({ pEntry1->GetSkillID(),  pEntry1 });
	////////////////////////////////20040221//////////////////////////////

	////////////////////////////////27111005//////////////////////////////
	pEntry1 = new SkillEntry;
	pEntry1->SetSkillID(27111005);
	pEntry1->SetMasterLevel(0);
	pEntry1->SetMaxLevel(10);
	pEntry1->AddLevelData(nullptr);
	for (int i = 1; i <= pEntry1->GetMaxLevel(); ++i)
	{
		d = i;

		SkillLevelData* pLevelData = new SkillLevelData;
		pLevelData->m_nTime = 60 + 12 * i;
		pLevelData->m_nMpCon = 56 + 2 * i;
		pLevelData->m_nIndiePdd = 100 + 20 * i;
		pLevelData->m_nIndieMdd = 100 + 20 * i;
		pLevelData->m_nProp = 2 * i;

		pEntry1->AddLevelData(pLevelData);
	}
	m_mSkillByRootID[2711]->insert({ pEntry1->GetSkillID(),  pEntry1 });
	////////////////////////////////27111005//////////////////////////////

	////////////////////////////////32120001//////////////////////////////
	pEntry1 = new SkillEntry;
	pEntry1->SetSkillID(32120001);
	pEntry1->SetMasterLevel(0);
	pEntry1->SetMaxLevel(30);
	pEntry1->AddLevelData(nullptr);
	for (int i = 1; i <= pEntry1->GetMaxLevel(); ++i)
	{
		d = i;
		SkillLevelData* pLevelData = new SkillLevelData;
		pLevelData->m_nMpCon = 20 + 5 * (int)ceil(d / 5);
		pLevelData->m_nX = 20 + (int)ceil(d / 5);
		pLevelData->m_nY = -1;
		pLevelData->m_nZ = 5 + (int)ceil(d / 5);
		pLevelData->m_nSpeed = -i;
		pLevelData->m_nMobCount = 6;
		pLevelData->m_nEr = 5 + (int)ceil(d / 2);
		//pLevelData->m_nLt

		pEntry1->AddLevelData(pLevelData);
	}
	m_mSkillByRootID[3212]->insert({ pEntry1->GetSkillID(),  pEntry1 });
	auto pSkill = m_mSkillByRootID[221]->operator[](2211010)->GetLevelData(5);
	////////////////////////////////32120001//////////////////////////////
}*/

int SkillInfo::GetSkillLevel(GA_Character * pCharacter, int nSkillID, SkillEntry ** pEntry, int bNoPvPLevelCheck, int bPureStealSLV, int bNotApplySteal, int bCheckSkillRoot)
{
	auto pSkillTreeByJob = m_mSkillByRootID.find(nSkillID / 10000);
	if (pSkillTreeByJob->second == nullptr)
		return 0;
	auto findIter = pSkillTreeByJob->second->find(nSkillID);
	if (findIter->second == nullptr)
		return 0;

	if(pEntry)
		*pEntry = findIter->second;
	auto characterSkillRecord = pCharacter->GetSkill(nSkillID);
	if (characterSkillRecord == nullptr)
		return 0;
	return characterSkillRecord->nSLV;
}

GW_SkillRecord * SkillInfo::GetSkillRecord(int nSkillID, int nSLV, long long int tExpired)
{
	auto pSkill = GetSkillByID(nSkillID);
	if (pSkill == nullptr)
		return nullptr;
	GW_SkillRecord* pRecord = new GW_SkillRecord;

	pRecord->nSkillID = nSkillID;
	pRecord->nSLV = nSLV;
	pRecord->nMasterLevel = pSkill->GetMasterLevel();
	pRecord->tExpired = tExpired;
	return pRecord;
}
