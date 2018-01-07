#include "SkillInfo.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_SkillRecord.h"
#include "ItemInfo.h"
#include "SkillEntry.h"
#include "..\WvsLib\WzResMan.hpp"
#include "SkillLevelData.h"
#include <thread>
#include "..\EquationEvaluator\Evaluator.h"

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
	printf("[SkillInfo::IterateSkillInfo]開始載入所有技能資訊 IterateSkillInfo Start.\n");
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
		printf("[SkillInfo::IterateSkillInfo]技能資訊載入完畢 IterateSkillInfo End.\n");
		LoadLevelDataSpecial();
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
		//int nTest = GetSkillByID(2111008)->GetLevelData(10)->m_nTime;
		//printf("Test 2111008 : %d\n", nTest);
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
	pResult->SetMasterLevel(skillDataImg["masterLevel"]);
	pResult->SetMaxLevel(skillCommonImg["maxLevel"]);
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
	Evaluator evaluator;
	double d;
	pEntry->AddLevelData(nullptr); //lvl 0
	for (int i = 1; i <= nMaxLevel; ++i)
	{
		d = i;

		SkillLevelData* pLevelData = new SkillLevelData();
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
		//parser.compile(((std::string)skillCommonImg["mpCon"]), expression);
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

		/*std::cout << "Test value : " << pLevelData->m_nMpCon << std::endl;
		if ((*pLevelData).m_nMpCon == 0)
			std::cout << "Test 2 value : " << pLevelData->m_nMpCon << std::endl;
		std::cout << "Test 3 value : " << pLevelData->m_nMpCon << std::endl;*/
		if (pLevelData->m_bConsumeOnPickup)
		{
			int party = 0;
			party = PARSE_SKILLDATA(party);
			pLevelData->m_bConsumeOnPickup = party > 0 ? 2 : pLevelData->m_bConsumeOnPickup;
		}

		pEntry->AddLevelData(pLevelData);
	}
}

void SkillInfo::LoadLevelDataByLevelNode(int nSkillID, SkillEntry * pEntry, void * pData)
{
	auto& skillLevelImg = *((WZ::Node*)pData);
	int nMaxLevel = pEntry->GetMaxLevel();
	pEntry->AddLevelData(nullptr); //for lvl 0

	double d = 0;
	Evaluator evaluator;
	for (auto& skillCommonImg : skillLevelImg)
	{
		d = (int)skillCommonImg;

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

void SkillInfo::LoadLevelDataSpecial()
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
		pLevelData->m_nMpCon = /*40+5*ceil(i/4)*/ PARSE_SKILLDATA_STRING(40+5*u(x/4));
		pLevelData->m_nDamage = 243+7*i;
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
		pLevelData->m_nIgnoreMobpdpR = /*40+5*ceil(i/4)*/ PARSE_SKILLDATA_STRING(5 + 5 * x);

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
}

int SkillInfo::GetSkillLevel(GA_Character * pCharacter, int nSkillID, SkillEntry ** pEntry, int bNoPvPLevelCheck, int bPureStealSLV, int bNotApplySteal, int bCheckSkillRoot)
{
	auto pSkillTreeByJob = m_mSkillByRootID.find(nSkillID / 10000);
	if (pSkillTreeByJob->second == nullptr)
		return 0;
	auto findIter = pSkillTreeByJob->second->find(nSkillID);
	if (findIter->second == nullptr)
		return 0;
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
