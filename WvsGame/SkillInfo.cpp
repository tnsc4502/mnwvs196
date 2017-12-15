
#include "exprtk.hpp"
#include "SkillInfo.h"
#include "..\Database\GA_Character.hpp"
#include "ItemInfo.h"
#include "SkillEntry.h"
#include "Wz\WzResMan.hpp"
#include "SkillLevelData.h"
#include <thread>

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
		return result;
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
		std::thread t(&SkillInfo::LoadSkillRoot, this, nRootID, (void*)(&node["skill"]));
		t.detach();
		//LoadSkillRoot(nRootID, (void*)(&node["skill"]));
	}
	//stWzResMan->ReleaseMemory();
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
		/*if (!skillImg) {
			printf("Skill Error : %d\n", nSkillRootID);
			continue;
		}*/
		nSkillID = atoi(skillImg.Name().c_str());
		std::thread t(&SkillInfo::LoadSkill, this, nSkillRootID, nSkillID, (void*)&skillImg);
		t.detach();
		//LoadSkill(nSkillRootID, nSkillID, (void*)&skillImg);
	}
}

SkillEntry * SkillInfo::LoadSkill(int nSkillRootID, int nSkillID, void * pData)
{
	++m_nOnLoadingSkills;
	auto& skillDataImg = *((WZ::Node*)pData);
	auto& skillCommonImg = skillDataImg["common"];
	auto& skillInfoImg = skillDataImg["info"];
	auto& skillListImg = skillDataImg["skillList"];
	auto& skillReqImg = skillDataImg["req"];
	SkillEntry* pResult = new SkillEntry;
	pResult->SetSkillID(nSkillID);
	pResult->SetMasterLevel(skillDataImg["masterLevel"]);
	pResult->SetMaxLevel(skillCommonImg["maxLevel"]);
	LoadLevelData(nSkillID, pResult, (void*)&skillCommonImg);
	
	std::lock_guard<std::mutex> lock(m_mtxSkillResLock);
	m_mSkillByRootID[nSkillRootID]->insert({ nSkillID, pResult });

	--m_nOnLoadingSkills;
	if (m_nOnLoadingSkills == 0) 
	{
		printf("[SkillInfo::IterateSkillInfo]技能資訊載入完畢 IterateSkillInfo End.\n");
		stWzResMan->ReleaseMemory();
	}
	return pResult;
}

void SkillInfo::LoadLevelData(int nSkillID, SkillEntry * pEntry, void * pData)
{
	auto& skillCommonImg = *((WZ::Node*)pData);
	int nMaxLevel = pEntry->GetMaxLevel();

	exprtk::parser<double> parser;
	double d = 0;
	for (int i = 0; i <= nMaxLevel; ++i)
	{
		d = i;
		exprtk::expression<double> expression;
		exprtk::symbol_table<double> symbol_table;
		//symbol_table.clear();
		symbol_table.add_variable("x", d, false);
		expression.register_symbol_table(symbol_table);

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
		//printf("Skill : %d, Level :%d, Parse Data : %s\r\n", nSkillID, i, ((std::string)skillCommonImg["ignoreMobpdpR"]).c_str());
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

		pEntry->AddLevelData(pLevelData);
	}
}
