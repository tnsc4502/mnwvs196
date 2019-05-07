#include "USkill.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_SkillRecord.h"

#include "User.h"
#include "TemporaryStat.h"
#include "SecondaryStat.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "SkillInfo.h"
#include "KaiserSkills.h"
#include "QWUSkillRecord.h"

#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"

#include "..\WvsLib\Logger\WvsLogger.h"


/*
This MACRO defines some variables that are required by REGISTER_TS
Please define this in the beginning section of every DoActiveSkill_XXX
*/
#define REGISTER_USE_SKILL_SECTION \
nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);\
auto pSkillLVLData = pSkill->GetLevelData(nSLV);\
auto tsFlag = TemporaryStat::TS_Flag::GetDefault();\
std::pair<long long int, std::vector<int*>>* pRef = nullptr;\
std::pair<long long int, std::vector<int>>* pRefIndie = nullptr;\
auto pSS = pUser->GetSecondaryStat();\
int nSkillID = pSkill->GetSkillID();\
int tDelay = 0;\
int nDuration = pSkillLVLData->m_nTime * 1000;\
	if (nDuration == 0)\
		nDuration = INT_MAX;


/*
此MACRO作為註冊TemporaryStat(TS)用。
每個TS一定都要先呼叫此MACRO後，再補上自己需要的屬性。
此限制帶來的好處是User::Update檢測每個TS的時候，可以查看時間是否已經逾時，也追蹤Reason技能(n##name)。
同時也會將n, r, t都放到m_mSetByTS中，並以TS_FLAG作為key。
當bResetBySkill為true時，表示以清除指定技能帶來的TS，會將n, r, t都設定為0，並且將m_mSetByTS對應的值清空。
*/

#define REGISTER_TS(name, value)\
tsFlag |= GET_TS_FLAG(##name);\
pRef = &pSS->m_mSetByTS[TemporaryStat::TS_##name]; pRef->second.clear();\
pSS->n##name = bResetBySkill ? 0 : value;\
pSS->r##name = bResetBySkill ? 0 : nSkillID;\
pSS->t##name = bResetBySkill ? 0 : nDuration;\
pSS->nLv##name = bResetBySkill ? 0 : nSLV;\
if(!bResetBySkill)\
{\
	pRef->first = bForcedSetTime ? nForcedSetTime : GameDateTime::GetTime();\
	pRef->second.push_back(&pSS->n##name);\
	pRef->second.push_back(&pSS->r##name);\
	pRef->second.push_back(&pSS->t##name);\
	pRef->second.push_back(&pSS->nLv##name);\
}\

#define REGISTER_INDIE_TS(name, value)\
tsFlag |= GET_TS_FLAG(##name);\
pRefIndie = &pSS->m_mSetByIndieTS[TemporaryStat::TS_##name][nSkillID]; pRefIndie->second.clear();\
pSS->n##name += bResetBySkill ? -value : value;\
if(!bResetBySkill)\
{\
	pRefIndie->first = bForcedSetTime ? nForcedSetTime : GameDateTime::GetTime();\
	pRefIndie->second.push_back(value);\
	pRefIndie->second.push_back(nSkillID);\
	pRefIndie->second.push_back(nDuration);\
	pRefIndie->second.push_back(nSLV);\
}\

void USkill::ValidateSecondaryStat(User * pUser)
{
	auto pSS = pUser->GetSecondaryStat();
	auto iter = pSS->m_mSetByTS.begin(),
		 iterEnd = pSS->m_mSetByTS.end();

	while (iter != iterEnd)
		if (iter->second.second.size() == 0)
		{
			pSS->m_mSetByTS.erase(iter);
			iter = pSS->m_mSetByTS.begin();
		}
		else
			++iter;

	//Indie TS
	auto indieMapIter = pSS->m_mSetByIndieTS.begin(),
		 indieMapEnd = pSS->m_mSetByIndieTS.end();

	while (indieMapIter != indieMapEnd)
	{
		auto indieTS = indieMapIter->second.begin();
		while (indieTS != indieMapIter->second.end())
		{
			//技能對應的Indie TS已經被取消，清除
			if(indieTS->second.second.size() == 0)
			{
				indieMapIter->second.erase(indieTS);
				indieTS = indieMapIter->second.begin();
			}
			else
				++indieTS;
		}

		//整個Indie TS為空，清除
		if (indieMapIter->second.size() == 0)
		{
			pSS->m_mSetByIndieTS.erase(indieMapIter);
			indieMapIter = pSS->m_mSetByIndieTS.begin();
		}
		else
			++indieMapIter;
	}
}

void USkill::OnSkillUseRequest(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
	int tRequestTime = iPacket->Decode4();
	int nSkillID = iPacket->Decode4();
	int nSpiritJavelinItemID = 0;
	int nSLV = iPacket->Decode1();
	auto pSkillEntry = SkillInfo::GetInstance()->GetSkillByID(nSkillID);
	auto pSkillRecord = pUser->GetCharacterData()->GetSkill(nSkillID);
	if (pSkillEntry == nullptr || pSkillRecord == nullptr)
	{
		SendFailPacket(pUser);
		return;
	}
	nSLV = nSLV > pSkillRecord->nSLV ? pSkillRecord->nSLV : nSLV;
	if (!pUser->GetField() || nSLV <= 0) 
	{
		SendFailPacket(pUser);
		return;
	}
	USkill::OnSkillUseRequest(
		pUser,
		iPacket,
		pSkillEntry,
		nSLV,
		false,
		false,
		0
	);
}

void USkill::OnSkillUseRequest(User * pUser, InPacket *iPacket, const SkillEntry * pEntry, int nSLV, bool bResetBySkill, bool bForceSetTime, int nForceSetTime)
{
	int nSkillID = pEntry->GetSkillID();
	if (SkillInfo::IsSummonSkill(nSkillID))
		DoActiveSkill_Summon(pUser, pEntry, nSLV, iPacket, bResetBySkill, bForceSetTime, nForceSetTime);
	else
		DoActiveSkill_SelfStatChange(pUser, pEntry, nSLV, iPacket, 0, bResetBySkill, bForceSetTime, nForceSetTime);
}

void USkill::OnSkillUpRequest(User * pUser, InPacket * iPacket)
{
	int tTime = iPacket->Decode4();
	int nSkillID = iPacket->Decode4();
	int nAmount = 1;
	if (iPacket->GetPacketSize() > 8)
		nAmount = iPacket->Decode1();
	if (nAmount <= 0)
		nAmount = 1;
	OnSkillUpRequest(pUser, nSkillID, nAmount, true, true);
}

void USkill::OnSkillUpRequest(User * pUser, int nSkillID, int nAmount, bool bDecSP, bool bCheckMasterLevel)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
	std::vector<GW_SkillRecord*> aChange;
	if (QWUSkillRecord::SkillUp(
		pUser,
		nSkillID,
		nAmount,
		bDecSP,
		bCheckMasterLevel,
		aChange))
	{
		pUser->ValidateStat();
		pUser->SendCharacterStat(false, BasicStat::BS_SP);
	}
	QWUSkillRecord::SendCharacterSkillRecord(pUser, aChange);
}

void USkill::OnSkillPrepareRequest(User * pUser, InPacket * iPacket)
{
}

void USkill::OnSkillCancelRequest(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
	int nSkillID = iPacket->Decode4();
	SkillEntry* pSkill = nullptr;
	int nSLV = SkillInfo::GetInstance()->GetSkillLevel(
		pUser->GetCharacterData(),
		nSkillID,
		&pSkill,
		0,
		0,
		0,
		0);
	if (pSkill)
	{
		USkill::OnSkillUseRequest(
			pUser,
			iPacket,
			pSkill,
			nSLV,
			true,
			false,
			0
		);
	}
}

void USkill::SendFailPacket(User* pUser)
{
}

void USkill::DoActiveSkill_SelfStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, int nOptionValue, bool bResetBySkill, bool bForcedSetTime, int nForcedSetTime)
{
	nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);
	REGISTER_USE_SKILL_SECTION;
	if (iPacket)
		tDelay = iPacket->Decode2();
	if (!pSkillLVLData) 
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[USkill::DoActiveSkill_SelfStatChange]異常的技能資訊，技能ID = %d，技能等級 = %d\n", pSkill->GetSkillID(), nSLV);
		return;
	}
	if (pSkillLVLData->m_nPad > 0)
	{
		REGISTER_TS(PAD, pSkillLVLData->m_nPad);
	}
	if (pSkillLVLData->m_nPdd > 0)
	{
		REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
	}
	if (pSkillLVLData->m_nMdd > 0)
	{
		REGISTER_TS(MDD, pSkillLVLData->m_nMdd);
	}
	if (pSkillLVLData->m_nMad > 0)
	{
		REGISTER_TS(MAD, pSkillLVLData->m_nMad);
	}
	if (pSkillLVLData->m_nAcc > 0)
	{
		REGISTER_TS(ACC, pSkillLVLData->m_nAcc);
	}
	if (pSkillLVLData->m_nEva > 0)
	{
		REGISTER_TS(EVA, pSkillLVLData->m_nEva);
	}
	if (pSkillLVLData->m_nCraft > 0)
	{
		REGISTER_TS(Craft, pSkillLVLData->m_nCraft);
	}
	if (pSkillLVLData->m_nSpeed > 0)
	{
		REGISTER_TS(Speed, pSkillLVLData->m_nSpeed);
	}
	if (pSkillLVLData->m_nJump > 0)
	{
		REGISTER_TS(Jump, pSkillLVLData->m_nJump);
	}
	auto iter = pSS->m_mSetByTS.begin();
	switch (nSkillID)
	{
		case KaiserSkills::TempestBlades_6110:
		case KaiserSkills::TempestBlades_6111_2:
		case KaiserSkills::AdvancedTempestBlades_6112:
		case KaiserSkills::AdvancedTempestBlades_6112_2:
		{
			pSS->sStopForceAtomInfo.CreateStopForceAtom(pUser, nSkillID);
			REGISTER_TS(StopForceAtomInfo, nSLV);
			break;
		}
		case 2111008:
		{
			REGISTER_TS(ElementalReset, pSkillLVLData->m_nX);
			break;
		}
		case 2121052:
		{
			REGISTER_TS(FireAura, pSkillLVLData->m_nX);
			break;
		}
		case 15111023:
		{
			REGISTER_TS(AsrR, pSkillLVLData->m_nAsrR);
			REGISTER_TS(TerR, pSkillLVLData->m_nTerR);
			break;
		}
		case 1321015:
		{
			REGISTER_TS(IgnoreTargetDEF, pSkillLVLData->m_nIgnoreMobpdpR);
			REGISTER_TS(BDR, pSkillLVLData->m_nBdR);
			break;
		}
		case 15111024: //ironclad
		{
			REGISTER_TS(DamAbsorbShield, pSkillLVLData->m_nY);
			break;
		}
		case 2001002: // magic guard
		case 12001001:
		case 22111001:
		{
			REGISTER_TS(MagicGuard, pSkillLVLData->m_nY);
			break;
		}
		case 24111003://Bad Luck Ward
		{
			REGISTER_TS(MaxHP, pSkillLVLData->m_nIndieMhpR);//indieMhpR/x
			REGISTER_TS(MaxMP, pSkillLVLData->m_nIndieMmpR);//indieMmpR/x
															// REGISTER_TS(ABNORMAL_STATUS_R, pSkillLVLData->m_nX);
															// REGISTER_TS(TerR,ret.info.get(MapleStatInfo.y);
			REGISTER_TS(AsrR, pSkillLVLData->m_nAsrR);//x
			REGISTER_TS(TerR, pSkillLVLData->m_nTerR);//y
			break;
		}
		case 2301003: // invincible
		{
			REGISTER_TS(Invincible, pSkillLVLData->m_nX);
			break;
		}
		case 35120000://Extreme proto
		case 35001002://mech proto

			break;
		case 9101004:
		case 9001004: // hide
		{
			REGISTER_TS(DarkSight, pSkillLVLData->m_nX);
			break;
		}
		case 13101006: // Wind Walk
		{
			//REGISTER_TS(WindWalk, pSkillLVLData->m_nX);
			break;
		}
		case 4330001:
		{
			REGISTER_TS(DarkSight, nSLV);
			break;
		}
		case 4001003: // Dark Sight
		case 14001003: // cygnus ds
		case 20031211:
		{
			REGISTER_TS(DarkSight, pSkillLVLData->m_nX); // d
			break;
		}
		case 4211003: // pickpocket
		{
			REGISTER_TS(PickPocket, pSkillLVLData->m_nX);
			break;
		}
		case 4211005: // mesoguard
		case 4201011:
		{
			REGISTER_TS(MesoGuard, pSkillLVLData->m_nX);
			break;
		}
		case 4111001: // mesoup
		{
			REGISTER_TS(MesoUp, pSkillLVLData->m_nX);
			break;
		}
		case 14111000: // cygnus
		{
			REGISTER_TS(ShadowPartner, pSkillLVLData->m_nX); // d
			break;
		}
		case 4211008:
		{
			REGISTER_TS(ShadowPartner, nSLV);
			break;
		}
		case 11101002: // All Final attack
		case 51100002:
		case 13101002:
		case 13100022:
		case 13100027:
		case 13110022:
		case 13110027:
		case 13120003:
		case 13120010:
		{
			REGISTER_TS(FinalAttackProp, pSkillLVLData->m_nX);
			break;
		}
		case 22161004:
		{
			REGISTER_TS(OnyxDivineProtection, pSkillLVLData->m_nX);
			break;
		}
		case 3101004: // soul arrow
		case 3201004:
		case 2311002: // mystic door - hacked buff icon
		case 35101005:
		case 13101003:
		case 13101024://new cyngus
		{
			REGISTER_TS(SoulArrow, pSkillLVLData->m_nX);
			break;
		}
		case 2321010:
		case 2221009:
		case 2121009:
		{
			//REGISTER_TS(BUFF_MASTERY, pSkillLVLData->m_nX);
			break;
		}
		case 2320011: // arcane aim
		case 2220010: // arcane aim
		case 2120010: // arcane aim
		{
			REGISTER_TS(ArcaneAim, pSkillLVLData->m_nX);
			break;
		}
		case 1211006: // wk charges
		case 1211004:
		case 1211008:
		case 1221004:
		case 11111007:
			// case 51111003: // Mihile's Radiant Charge
		case 21101006:
		case 21111005:
		case 15101006:
		{
			REGISTER_TS(WeaponCharge, pSkillLVLData->m_nX);
			REGISTER_TS(DamR, pSkillLVLData->m_nZ);
			break;
			//                    case 51111004:
			//                        REGISTER_TS(ABNORMAL_STATUS_R, pSkillLVLData->m_nY);
			//                        REGISTER_TS(ELEMENTAL_STATUS_R, pSkillLVLData->m_nZ);
			//                        REGISTER_TS(DEFENCE_BOOST_R, pSkillLVLData->m_nX);
			//                        break;
			//                    case 51121006:
			//                        REGISTER_TS(DAMAGE_BUFF, pSkillLVLData->m_nX);
			//                        REGISTER_TS(HowlingCritical, pSkillLVLData->m_ncriticaldamageMin.x);
			//                        REGISTER_TS(HowlingCritical, pSkillLVLData->m_ncriticaldamageMax.x);
			//                        break;
		}
		case 2211008:
		case 12101005:
		case 22121001: // Elemental Reset
		{
			REGISTER_TS(ElementalReset, pSkillLVLData->m_nX);
			break;
		}
		case 3111000:
		case 3121008:
		case 13111001:
		{
			REGISTER_TS(Concentration, pSkillLVLData->m_nX);
			break;
		}
		case 5110001: // Energy Charge
		case 15100004:
		{
			REGISTER_TS(EnergyCharged, 0);
			break;
		}
		case 1101004:
		case 1201004:
		case 1301004:
		case 2111005:
		case 2211005:
		case 2311006:
		case 3101002:
		case 3201002:
		case 4101003:
		case 4201002:
		case 5101006:
		case 5201003:
		case 5301002:
		case 5701005:
		case 11101001:
		case 23101002:
		case 12101004:
		case 13101001:
		case 14101002:
		case 15101002:
		case 22141002:
		case 24101005:
		case 27101004:
		case 32101005:
		case 33001003:
		case 35101006:
		{
			REGISTER_TS(Booster, pSkillLVLData->m_nX * 2);
		}
		break;
		case 21001003: // polearm booster
		{
			REGISTER_TS(Booster, -pSkillLVLData->m_nY);
			break;
		}
		case 35111013:
		case 5111007:
		case 5211007:
		case 5811007:
		case 5911007:
		case 5311005:
		case 5320007:
		case 5120012:
		case 5220014:
		case 5711011:
		case 5720005:
		case 15111011:
		{
			REGISTER_TS(Dice, 0);
			break;
		}
		case 5120011:
		case 5220012:
		{
			//REGISTER_TS(DAMAGE_RATE, (int)ret.info.get(MapleStatInfo.damR); //i think
			break;
		}
		case 5121009:
		case 15111005:
		{
			REGISTER_TS(Speed, pSkillLVLData->m_nX);
			break;
		}
		case 4321000: //tornado spin uses same buffstats
		{
			REGISTER_TS(Dash_Speed, 100 + pSkillLVLData->m_nX);
			REGISTER_TS(Dash_Jump, pSkillLVLData->m_nY); //always 0 but its there
			break;
		}
		case 5001005: // Dash
		case 15001003:
		{
			REGISTER_TS(Dash_Speed, pSkillLVLData->m_nX);
			REGISTER_TS(Dash_Jump, pSkillLVLData->m_nY);
			break;
		}
		case 1101007: // pguard
		case 1201007:
		{
			REGISTER_TS(PowerGuard, pSkillLVLData->m_nX);
			break;
		}
		case 32111004: //conversion
		{
			REGISTER_TS(Conversion, pSkillLVLData->m_nX);
			break;
		}
		case 1301007: // hyper body
		case 9001008:
		case 9101008:
		{
			REGISTER_TS(MaxHP, pSkillLVLData->m_nX);
			REGISTER_TS(MaxMP, pSkillLVLData->m_nX);
			break;
		}
		case 1111002: // combo
		case 11111001: // combo
		case 1101013:
		{
			REGISTER_TS(ComboCounter, 1);
			//  System.out.println("Combo just buff stat");
			break;
		}
		case 21120007: //combo barrier
		{
			REGISTER_TS(ComboBarrier, pSkillLVLData->m_nX);
			break;
		}
		case 5211006: // Homing Beacon
		case 5220011: // Bullseye
		case 22151002: //killer wings
		{
			REGISTER_TS(StopForceAtomInfo, pSkillLVLData->m_nX);
			break;
		}
		case 1311015: // Cross Surge
		{
			//REGISTER_TS(CROSS_SURGE, pSkillLVLData->m_nX);
			break;
		}
		case 21111009: //combo recharge
		case 1311006: //dragon roar
		case 1311005: //NOT A BUFF - Sacrifice
			//ret.hpR = -ret.info.get(MapleStatInfo.x) / 100.0;
			break;
		case 1211010: //NOT A BUFF - HP Recover
			//ret.hpR = ret.info.get(MapleStatInfo.x) / 100.0;
			break;
		case 4341002:
		{
			//ret.hpR = -ret.info.get(MapleStatInfo.x) / 100.0;
			REGISTER_TS(FinalCut, pSkillLVLData->m_nY);
			break;
		}
		case 2111007:
		case 2211007:
		case 2311007:
		case 32111010:
		case 22161005:
		case 12111007:
		{

			REGISTER_TS(TeleportMasteryOn, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.STUN, 1);
			break;
		}
		case 4331003:
		{
			//REGISTER_TS(OWL_SPIRIT, pSkillLVLData->m_nY);
			break;
		}
		case 1311008: // dragon blood
					  // REGISTER_TS(DRAGONBLOOD, pSkillLVLData->m_nX);
			break;
		case 5321005:
		case 1121000: // maple warrior, all classes
		case 5721000:
		case 1221000:
		case 1321000:
		case 2121000:
		case 2221000:
		case 2321000:
		case 3121000:
		case 3221000:
		case 4121000:
		case 4221000:
		case 5121000:
		case 5221000:
		case 21121000: // Aran - Maple Warrior
		case 32121007:
		case 35121007:
		case 23121005:
		case 24121008: // phantom
		case 100001268: // Zero
						// case 51121005: //Mihile's Maple Warrior
			REGISTER_INDIE_TS(IndiePAD, pSkillLVLData->m_nX);
			//REGISTER_TS(BasicStatUp, pSkillLVLData->m_nX);
			break;
		case 15111006: //spark
			//REGISTER_TS(SPARK, pSkillLVLData->m_nX);
			break;
		case 3121002: // sharp eyes bow master
		case 3221002: // sharp eyes marksmen
			REGISTER_TS(SharpEyes, (pSkillLVLData->m_nX << 8) + (pSkillLVLData->m_nCriticaldamageMax));
			break;
		case 22151003: //magic resistance
			REGISTER_TS(MagicResistance, pSkillLVLData->m_nX);
			break;
		case 2000007:
		case 12000006:
		case 22000002:
		case 32000012:
			REGISTER_TS(WeaknessMdamage, pSkillLVLData->m_nX);
			break;
		case 21101003: // Body Pressure
			REGISTER_TS(BodyPressure, pSkillLVLData->m_nX);
			break;
		case 21000000: // Aran Combo
			REGISTER_TS(ComboAbilityBuff, 100);
			break;
		case 23101003:
			REGISTER_TS(DamR, pSkillLVLData->m_nDamage);
			REGISTER_TS(CriticalBuff, pSkillLVLData->m_nX);
			break;
		case 24121054:
		{
			REGISTER_TS(ComboDrain, pSkillLVLData->m_nZ);
			REGISTER_TS(CriticalBuff, pSkillLVLData->m_nV);

			//    REGISTER_TS(ABNORMAL_STATUS_R, pSkillLVLData->m_nX);//x
			//    REGISTER_TS(ELEMENTAL_STATUS_R, pSkillLVLData->m_nY);//y
			break;
		}
		case 21100005: // Combo Drain
		case 32101004:
		{
			REGISTER_TS(ComboDrain, pSkillLVLData->m_nX);
			break;
		}
		case 41101003: // Military Might
		{
			REGISTER_TS(EMHP, pSkillLVLData->m_nX);
			REGISTER_TS(EMMP, pSkillLVLData->m_nY);
			REGISTER_TS(EPAD, pSkillLVLData->m_nPadX);
			REGISTER_TS(Speed, pSkillLVLData->m_nSpeed);
			REGISTER_TS(Jump, pSkillLVLData->m_nJump);
			break;
		}
		case 15001022:
		{
			//REGISTER_TS(LIGHTNING, pSkillLVLData->m_nV);
			REGISTER_TS(IgnoreTargetDEF, pSkillLVLData->m_nX);
			break;
		}
		case 41110008:
		case 41001001: // Battoujutsu Stance
		{
			REGISTER_TS(CriticalBuff, pSkillLVLData->m_nY);
			//REGISTER_TS(BATTOUJUTSU_STANCE, 1);
			REGISTER_TS(Booster, pSkillLVLData->m_nActionSpeed);
			//REGISTER_TS(HAYATO_STANCE, pSkillLVLData->m_nProp);
			break;
		}
		case 41110009: // Warrior's Heart
		{
			REGISTER_TS(Regen, pSkillLVLData->m_nDamage);
			break;
		}
		case 42101020:
		{
			REGISTER_TS(Regen, pSkillLVLData->m_nHp);
			break;
		}
		case 23111004: // ignis roar
		case 23121054:
		{
			REGISTER_TS(IgnisRore, pSkillLVLData->m_nX);
			// REGISTER_TS(IndiePAD, (int) ret.info.get(MapleStatInfo.indiePad);
			break;
		}
		case 21111001: // Smart Knockback
		{
			REGISTER_TS(KnockBack, pSkillLVLData->m_nX);
			break;
		}
		case 23121004:// ancient warding
		{
			REGISTER_TS(EMHP, pSkillLVLData->m_nEmhp);
			REGISTER_TS(DamR, pSkillLVLData->m_nDamR);
			break;
		}
		case 1211009:
		case 1111007:
		case 1311007: //magic crash
		case 51111005: //Mihile's magic crash
			//ret.monsterStatus.put(MonsterStatus.MOB_STAT_MagicCrash, 1);
			break;
		case 1220013:
		{
			REGISTER_TS(BlessingArmor, pSkillLVLData->m_nX + 1);
			break;
		}
		case 1211011: 
		{
			REGISTER_TS(CombatOrders, pSkillLVLData->m_nX);
			break;
		}
		case 23111005: // water shield 
		{
			REGISTER_TS(AsrR, pSkillLVLData->m_nTerR);
			REGISTER_TS(TerR, pSkillLVLData->m_nTerR);
			REGISTER_TS(DamAbsorbShield, pSkillLVLData->m_nX);
			break;
		}
		case 22131001: //magic shield
			REGISTER_TS(MagicShield, pSkillLVLData->m_nX);
			break;
		case 27121005: // Dark Crescendo TODO: Count up GMS-Like
			
			REGISTER_TS(StackBuff, pSkillLVLData->m_nX);
			break;
		case 22181003: //soul stone
			REGISTER_TS(SoulStone, 1);
			break;
		case 24111002: //Final Feint
			
			REGISTER_TS(SoulStone, 1);
			break;
		case 32121003: //twister
			//REGISTER_TS(TORNADO, pSkillLVLData->m_nX);
			break;
		case 2311009: //holy magic
			REGISTER_TS(HolyMagicShell, pSkillLVLData->m_nX);
			
			//ret.hpR = ret.info.get(MapleStatInfo.z) / 100.0;
			break;
		case 32111005: //body boost
			
			REGISTER_TS(DamR, pSkillLVLData->m_nDamR); //lots of variables
			break;
		case 22131002:
		case 22141003: // Slow
			REGISTER_TS(Slow, pSkillLVLData->m_nX);
			break;
		case 4001002: // disorder
		case 14001002: // cygnus disorder
			//ret.monsterStatus.put(MonsterStatus.WATK, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.WDEF, pSkillLVLData->m_nY);
			break;
		case 5221009: // Mind Control
			//ret.monsterStatus.put(MonsterStatus.HYPNOTIZE, 1);
			break;
		case 4341003: // Monster Bomb
					  //    //ret.monsterStatus.put(MonsterStatus.MONSTER_BOMB, (int) ret.info.get(MapleStatInfo.damage);
			break;
		case 1201006: // threaten
			//ret.monsterStatus.put(MonsterStatus.WATK, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.WDEF, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.DARKNESS, pSkillLVLData->m_nZ);
			break;
		case 22141001:
		case 1211002: // charged blow
		case 1111008: // shout
		case 4211002: // assaulter
		case 3101005: // arrow bomb
		case 1111005: // coma: sword
		case 4221007: // boomerang step
		case 5101002: // Backspin Blow
		case 5101003: // Double Uppercut
		case 5121004: // Demolition
		case 5121005: // Snatch
		case 5121007: // Barrage
		case 5201004: // pirate blank shot
		case 4121008: // Ninja Storm
		case 22151001:
		case 4201004: //steal, new
		case 33101001:
		case 33101002:
		case 32101001:
		case 32111011:
		case 32121004:
		case 33111002:
		case 33121002:
		case 35101003:
		case 35111015:
		case 5111002: //energy blast
		case 15101005:
		case 4331005:
		case 1121001: //magnet
		case 1221001:
		case 1321001:
		case 9001020:
		case 31111001:
		case 31101002:
		case 9101020:
		case 2211003:
		case 2311004:
		case 3120010:
		case 22181001:
		case 21110006:
		case 22131000:
		case 5301001:
		case 5311001:
		case 5311002:
		case 2221006:
		case 5310008:
		case 27121052:
			//ret.monsterStatus.put(MonsterStatus.STUN, 1);
			break;
		case 90001004:
		case 4321002:
		case 1111003:
		case 11111002:
			//ret.monsterStatus.put(MonsterStatus.DARKNESS, pSkillLVLData->m_nX);
			break;
		case 4221003:
		case 4121003:
		case 33121005:
			//ret.monsterStatus.put(MonsterStatus.SHOWDOWN, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.MDEF, pSkillLVLData->m_nX); // removed for taunt
			//ret.monsterStatus.put(MonsterStatus.WDEF, pSkillLVLData->m_nX); // removed for taunt
			break;
		case 31121003:
			//ret.monsterStatus.put(MonsterStatus.SHOWDOWN, pSkillLVLData->m_nW);
			//ret.monsterStatus.put(MonsterStatus.MDEF, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.WDEF, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.MATK, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.WATK, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.ACC, pSkillLVLData->m_nX);
			break;
		case 23121002: //not sure if negative
			//ret.monsterStatus.put(MonsterStatus.WDEF, -ret.info.get(MapleStatInfo.x);
			break;
		case 2201004: // cold beam
		case 2221003:
		case 2211002: // ice strike
		case 3211003: // blizzard
		case 2211006: // il elemental compo
		case 2221007: // Blizzard
		case 5211005: // Ice Splitter
		case 2121006: // Paralyze
		case 21120006: // Tempest
		case 22121000:
		case 90001006:
		case 2221001:
			////ret.monsterStatus.put(MonsterStatus.FREEZE, 1);
			//
			break;
		case 2101003: // fp slow
		case 2201003: // il slow
		case 12101001:
		case 90001002:
			////ret.monsterStatus.put(MonsterStatus.SPEED, pSkillLVLData->m_nX);
			break;
		case 5011002:
			////ret.monsterStatus.put(MonsterStatus.SPEED, pSkillLVLData->m_nZ);
			break;
		case 1121010: //enrage
			REGISTER_TS(Enrage, pSkillLVLData->m_nX * 100 + pSkillLVLData->m_nMobCount);
			break;
		case 23111002: //TODO LEGEND: damage increase?
		case 22161002: //phantom imprint
			////ret.monsterStatus.put(MonsterStatus.IMPRINT, pSkillLVLData->m_nX);
			break;
		case 90001003:
			////ret.monsterStatus.put(MonsterStatus.POISON, 1);
			break;
		case 4121004: // Ninja ambush
		case 4221004:
			////ret.monsterStatus.put(MonsterStatus.NINJA_AMBUSH, (int)ret.info.get(MapleStatInfo.damage);
			break;
		case 2311005:
			////ret.monsterStatus.put(MonsterStatus.DOOM, 1);
			break;
		case 32111006:
			//REGISTER_TS(REAPER, 1);
			break;
		case 35111001:
		case 35111010:
		case 35111009:
			
			//REGISTER_TS(PUPPET, 1);
			break;
		case 80001155: // Terms and Conditions
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			break;
		case 4341006:
		case 3120012:
		case 3220012:
		case 3111002: // puppet ranger
		case 3211002: // puppet sniper
		case 13111004: // puppet cygnus
		case 13111024: // Emerald Flower
		case 5211001: // Pirate octopus summon
		case 5220002: // wrath of the octopi
		case 33111003:
		case 5321003:
		case 5211014:
			//REGISTER_TS(PUPPET, 1);
			break;
		case 3120006:
		case 3220005:
			REGISTER_TS(TerR, pSkillLVLData->m_nTerR);
			REGISTER_TS(SpiritLink, 1);
			break;
		case 5220019:
			
			break;
		case 5211011:
		case 5211015:
		case 5211016:
		case 5711001: // turret
		case 2121005: // elquines
		case 3201007:
		case 3101007:
		case 3211005: // golden eagle
		case 3111005: // golden hawk
		case 33111005:
		case 35111002:
		case 3121006: // phoenix
		case 23111008:
		case 23111009:
		case 23111010:
			//REGISTER_TS(SUMMON, 1);
			////ret.monsterStatus.put(MonsterStatus.STUN, 1);
			break;
		case 3221005: // frostprey
		case 2221005: // ifrit
			//REGISTER_TS(SUMMON, 1);
			////ret.monsterStatus.put(MonsterStatus.FREEZE, 1);
			break;
		case 35111005:
			//REGISTER_TS(SUMMON, 1);
			////ret.monsterStatus.put(MonsterStatus.SPEED, pSkillLVLData->m_nX);
			////ret.monsterStatus.put(MonsterStatus.WDEF, pSkillLVLData->m_nY);
			break;
		case 1321007: // Beholder
		case 1301013: // Evil Eye
		case 1311013: // Evil Eye of Domination
			REGISTER_TS(Beholder, nSLV);
			break;
		case 36121002:
		case 36121013:
		case 36121014:
			//REGISTER_TS(EVAR, 1);
			//REGISTER_TS(SUMMON, 1);
			break;
		case 65101002:
			REGISTER_TS(DamAbsorbShield, pSkillLVLData->m_nX);
			break;
		case 65111004: // Iron Blossom
			REGISTER_TS(Stance, pSkillLVLData->m_nProp);
			break;
		case 35121010:
			
			REGISTER_TS(DamR, pSkillLVLData->m_nX);
			break;
		case 31121005:
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nDamR);
			//REGISTER_TS(DARK_METAMORPHOSIS, 6); // mob count
			break;
		case 2311003: // hs
		case 9001002: // GM hs
		case 9101002:
			REGISTER_TS(HolySymbol, pSkillLVLData->m_nX);
			break;
		case 40011288:
			REGISTER_TS(HayatoChangeMode, pSkillLVLData->m_nX);
			break;
		case 40011291:
			REGISTER_TS(HayatoChangeMode, pSkillLVLData->m_nX);
			break;
		case 80001034: //virtue
		case 80001035: //virtue
		case 80001036: //virtue
			//REGISTER_TS(VIRTUE_EFFECT, 1);
			break;
		case 2211004: // il seal
		case 2111004: // fp seal
		case 12111002: // cygnus seal
		case 90001005:
			////ret.monsterStatus.put(MonsterStatus.SEAL, 1);
			break;
		case 24121003:
			break;
		case 4111003: // shadow web
		case 14111001:
			////ret.monsterStatus.put(MonsterStatus.SHADOW_WEB, 1);
			break;
		case 4111009: // Shadow Stars
		case 5201008:
		case 14111007:
			REGISTER_TS(NoBulletConsume, 0);
			break;
		case 2121004:
		case 2221004:
		case 2321004: // Infinity
			//ret.hpR = ret.info.get(MapleStatInfo.y) / 100.0;
			//ret.mpR = ret.info.get(MapleStatInfo.y) / 100.0;
			REGISTER_TS(Infinity, pSkillLVLData->m_nX);
			REGISTER_TS(Stance, pSkillLVLData->m_nProp);
			break;
		case 22181004:
			//REGISTER_TS(ONYX_WILL, pSkillLVLData->m_nDamage); //is this the right order
			REGISTER_TS(Stance, pSkillLVLData->m_nProp);
			break;
		case 1121002:
		case 1221002:
		case 1321002: // Stance
					  // case 51121004: //Mihile's Stance
		case 50001214:
		case 80001140:
		case 21121003: // Aran - Freezing Posture
		case 32121005:
		case 5321010:
			REGISTER_TS(Stance, pSkillLVLData->m_nProp);
			break;
		case 2121002: // mana reflection
		case 2221002:
		case 2321002:
			REGISTER_TS(ManaReflection, 1);
			break;
		case 2321005: // holy shield, TODO Jump
					  //                            REGISTER_TS(AdvancedBless, GameConstants.GMS ? (int) ret.level : ret.info.get(MapleStatInfo.x);
			REGISTER_TS(AdvancedBless, pSkillLVLData->m_nX);
			REGISTER_TS(IncMaxHP, pSkillLVLData->m_nY);//fix names
			REGISTER_TS(IncMaxMP, pSkillLVLData->m_nZ);
			break;
		case 3121007: // Hamstring
			REGISTER_TS(IllusionStep, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.SPEED, pSkillLVLData->m_nX);
			break;
		case 3221006: // Blind
		case 33111004:
			REGISTER_TS(Blind, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.ACC, pSkillLVLData->m_nX);
			break;

		case 9101003: //customs for infinite dmg :D
			REGISTER_INDIE_TS(IndiePAD, INT_MAX);
			REGISTER_TS(IndieMAD, INT_MAX);
			REGISTER_TS(IndieMaxDamageOver, 500000);
		case 2301004:
		case 9001003:
			REGISTER_TS(Bless, nSLV);
			break;
		case 32120000:
			
			
		case 32001003: //dark aura
		case 32110007:
			
			//REGISTER_TS(AURA, nSLV);
			REGISTER_TS(BMageAura, pSkillLVLData->m_nX);
			break;
		case 32111012: //blue aura
		case 32110000:
		case 32110008:
			
			//REGISTER_TS(AURA, (int)ret.level);
			REGISTER_TS(BMageAura, pSkillLVLData->m_nX);
			break;
		case 32120001:
			//ret.monsterStatus.put(MonsterStatus.SPEED, pSkillLVLData->m_nspeed);
		case 32101003: //yellow aura
		case 32110009:
			
			//REGISTER_TS(AURA, pSkillLVLData->m_nX);
			REGISTER_TS(BMageAura, pSkillLVLData->m_nX);
			break;
		case 33101004: //it's raining mines
			//REGISTER_TS(RAINING_MINES, pSkillLVLData->m_nX); //x?
			break;
		case 35101007: //perfect armor
			
			REGISTER_TS(GUARD, pSkillLVLData->m_nX);
			break;
		case 35121006: //satellite safety
			
			//REGISTER_TS(SATELLITESAFE_PROC, pSkillLVLData->m_nX);
			//REGISTER_TS(SATELLITESAFE_ABSORB, pSkillLVLData->m_nY);
			break;
		case 80001040:
		case 20021110:
		case 20031203:
			//ret.moveTo = ret.info.get(MapleStatInfo.x);
			break;
		case 5311004:
			REGISTER_TS(RepeatEffect, 0);
			break;
		case 5121015:
			REGISTER_TS(DamR, pSkillLVLData->m_nX);
			break;
		case 80001089: // Soaring
			
			REGISTER_TS(Flying, 1);
			break;
		case 20031205:
			//REGISTER_TS(PHANTOM_MOVE, pSkillLVLData->m_nX);
			break;
		case 5211009:
			
			break;
		case 35001001: //flame
		case 35101009:
			
			REGISTER_TS(Mechanic, nSLV); //ya wtf
			break;
		case 35121013:
		case 35111004: //siege
					   //
			
		case 35111003: //missile
			
			REGISTER_TS(Mechanic, nSLV); //ya wtf
			break;
		case 35111016: // overclock mech
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			REGISTER_TS(IgnoreTargetDEF, pSkillLVLData->m_nX);
			break;
		case 10001075: // Cygnus Echo
		case 50001075: // Mihile's Empress's Prayer
			REGISTER_TS(MaxLevelBuff, pSkillLVLData->m_nX);
			break;

		case 11001022:
			//REGISTER_TS(SUMMON, 1);
			break;
		case 11001021:
			break;
		case 11101024:
			REGISTER_TS(Booster, pSkillLVLData->m_nX);
			break;
		case 11121006:
			break;
		case 11121054:
			REGISTER_TS(IndieMaxDamageOver, pSkillLVLData->m_nIndieMaxDamageOver);
			break;
		case 4111002:
		case 4331002:
		case 15121004:
			REGISTER_TS(ShadowPartner, pSkillLVLData->m_nX);
			break;
		case 15121054:
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			break;
		case 30021237:
			REGISTER_TS(NaviFlying, 1);
			
			break;
		case 2321054:
			//REGISTER_TS(ANGEL, 1);
			REGISTER_TS(IgnoreTargetDEF, pSkillLVLData->m_nIgnoreMobpdpR);
			REGISTER_TS(IndieBooster, pSkillLVLData->m_nIndieBooster);
			REGISTER_TS(IndieMAD, pSkillLVLData->m_nIndieMad);
			REGISTER_TS(IndieMaxDamageOver, pSkillLVLData->m_nIndieMaxDamageOver);
			break;
			//    case 4211008:
		case 36111006:
			REGISTER_TS(ShadowPartner, nSLV);
			break;
		case 13111023:
			REGISTER_TS(Albatross, pSkillLVLData->m_nX);
			REGISTER_TS(IndiePAD, pSkillLVLData->m_nIndiePad);
			REGISTER_TS(IncMaxHP, pSkillLVLData->m_nIndieMhp);
			REGISTER_TS(IndieBooster, pSkillLVLData->m_nIndieBooster);//true?
			REGISTER_TS(IndieCr, pSkillLVLData->m_nIndieCr);
			break;
		case 13120008:
			REGISTER_TS(Albatross, pSkillLVLData->m_nX);
			REGISTER_TS(IndiePAD, pSkillLVLData->m_nIndiePad);
			REGISTER_TS(IncMaxHP, pSkillLVLData->m_nIndieMhp);
			REGISTER_TS(IndieBooster, pSkillLVLData->m_nIndieBooster);//true?
			REGISTER_TS(IndieCr, pSkillLVLData->m_nIndieCr);
			break;
		case 36001005 /*尖兵.精准火箭*/:
			
			REGISTER_TS(PreciseMissle, pSkillLVLData->m_nX);
			break;

		case 4311009:
		case 15101022:
		case 31201002:
		case 31001001:
		case 36101004:
		case 41101005:
		case 42101003:
		case 51101003:
			REGISTER_TS(Booster, pSkillLVLData->m_nX);
			break;
		case 41121003:
			REGISTER_TS(AsrR, pSkillLVLData->m_nX);
			REGISTER_TS(TerR, pSkillLVLData->m_nY);
			break;
		case 31211003:
			REGISTER_TS(AsrR, pSkillLVLData->m_nX);
			REGISTER_TS(TerR, pSkillLVLData->m_nY);
			break;
		case 36111008: // Emergency Resupply
			REGISTER_TS(SurplusSupply, pSkillLVLData->m_nX);
			break;
		case 27101202:
			
			REGISTER_TS(KeyDownAreaMoving, pSkillLVLData->m_nX);
			break;
		case 27111004:
			
			REGISTER_TS(KeyDownAreaMoving, 3);
			break;
		case 27111006:
			REGISTER_TS(EMMP, -pSkillLVLData->m_nEmmp);
			break;
		case 30010242:
			REGISTER_TS(Larkness, 1);
			break;
			//   case 27121052:
			//       //ret.monsterStatus.put(MonsterStatus.STUN, Integer.valueOf(1);
			//   case 27121054:
			//       REGISTER_TS(LUMINOUS_GAUGE, Integer.valueOf(20040218);
			//       break;
		case 27121006: // Arcane Pitch
			REGISTER_TS(IgnoreTargetDEF, pSkillLVLData->m_nX);
			REGISTER_TS(FinalAttackProp, pSkillLVLData->m_nY);
			break;
		case 24121053:
		case 21121053:
		case 22171053:
		case 23121053:
		case 27121053: //Heroic Memories
			REGISTER_TS(IndieMaxDamageOver, pSkillLVLData->m_nIndieMaxDamageOver);
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			break;
		case 30020234:
			REGISTER_TS(Stance, pSkillLVLData->m_nW);
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nZ);
			REGISTER_TS(EVA, pSkillLVLData->m_nY);
			break;
		case 36000004:
			REGISTER_TS(Stance, pSkillLVLData->m_nW);
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nZ);
			REGISTER_TS(EVA, pSkillLVLData->m_nY);
			break;
		case 36100007:
			REGISTER_TS(Stance, pSkillLVLData->m_nW);
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nZ);
			REGISTER_TS(EVA, pSkillLVLData->m_nY);
			break;
		case 36110004:
			REGISTER_TS(Stance, pSkillLVLData->m_nW);
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nZ);
			REGISTER_TS(EVA, pSkillLVLData->m_nY);
			break;
		case 36120010:
			REGISTER_TS(Stance, pSkillLVLData->m_nW);
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nZ);
			REGISTER_TS(EVA, pSkillLVLData->m_nY);
			break;
		case 36120016:
			REGISTER_TS(Stance, pSkillLVLData->m_nW);
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nZ);
			REGISTER_TS(EVA, pSkillLVLData->m_nY);
			break;

		case 5721053:
		case 5321053:
		case 5121053:
		case 4341053:
		case 4221053:
		case 4121053:
		case 3221053:
		case 3121053:
		case 2321053:
		case 2221053:
		case 2121053:
		case 1321053:
		case 1221053:
		case 1121053: //Epic Adventure
			REGISTER_INDIE_TS(IndieMaxDamageOver, pSkillLVLData->m_nIndieMaxDamageOver);
			REGISTER_INDIE_TS(IndiePAD, pSkillLVLData->m_nIndieDamR);
			break;
		case 31221053:
		case 31121053:
		case 32121053:
		case 33121053:
			REGISTER_TS(IndieMaxDamageOver, pSkillLVLData->m_nIndieMaxDamageOver);
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			break;
		case 5221053://epic Aventure corsair
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			REGISTER_TS(IndieMaxDamageOver, pSkillLVLData->m_nIndieMaxDamageOver);
			break;
		case 51121053: //Queen of Tomorrow Mihile
		case 35121053://for liberty mechanic
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			REGISTER_TS(IndieMaxDamageOver, pSkillLVLData->m_nIndieMaxDamageOver);
			break;
		case 61101004:
			REGISTER_TS(Booster, -pSkillLVLData->m_nX);
			break;
		case 61111003:
			REGISTER_TS(AsrR, pSkillLVLData->m_nAsrR);
			REGISTER_TS(TerR, pSkillLVLData->m_nTerR);
			break;
		case 60001216:
			
		//	REGISTER_TS(KAISER_MODE_CHANGE, Integer.valueOf(1);
			break;
		case 60001217:
			//  case 61100005:
			
		//	REGISTER_TS(KAISER_MODE_CHANGE, Integer.valueOf(0);
			break;
		case 61111004:
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			break;
		case 61111008: // final form
		case 61120008: // final form
		case 61121053: // final trance
			REGISTER_TS(Speed, -pSkillLVLData->m_nSpeed);
			REGISTER_TS(Morph, pSkillLVLData->m_nMorph);
			REGISTER_TS(CriticalBuff, -pSkillLVLData->m_nCr);
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			REGISTER_TS(IndieBooster, pSkillLVLData->m_nIndieBooster);
			break;
		case 61121054: // kaiser's majesty
			//REGISTER_TS(KAISER_MAJESTY3, pSkillLVLData->m_nX);
			//REGISTER_TS(KAISER_MAJESTY4, pSkillLVLData->m_nX);
			REGISTER_TS(IndiePAD, INT_MAX);
			REGISTER_TS(IndieBooster, pSkillLVLData->m_nIndieBooster);
			break;
		case 4341054: // blade clone
			//REGISTER_TS(ARIANT_COSS_IMU2, Integer.valueOf(1);
		//	ret.overTime = true;
			break;
		case 4341052: // asura's anger
			REGISTER_TS(Asura, pSkillLVLData->m_nX);
			break;
		case 35120014: // double down
			REGISTER_TS(Dice, 1);
			break;
		case 80001264: // warring states manifesto
			
			break;
		case 4121054: // bleed dart
			REGISTER_TS(Speed, 1);
			break;
		case 31211004: // diabolic recovery
			
			REGISTER_TS(IndieMHPR, pSkillLVLData->m_nIndieMhpR);
			REGISTER_TS(DiabolikRecovery, pSkillLVLData->m_nX);
			break;
		case 31221004: // overwhelming power
			REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			REGISTER_TS(IndieBooster, 2);
			break;
		case 36101003:
			REGISTER_TS(IndieMMPR, pSkillLVLData->m_nIndieMmpR);
			REGISTER_TS(IndieMHPR, pSkillLVLData->m_nIndieMhpR);
			
			break;
		case 61121009: //
			REGISTER_TS(PartyBarrier, pSkillLVLData->m_nIgnoreMobpdpR);
			break;
		case 110001501:
		case 110001502:
		case 110001503:
		case 110001504:
			REGISTER_TS(BeastModeChange, (nSkillID - 110001501 + 1));
			break;

		case 4341000:
		case 22171000:
		case 13121000:
		case 11121000:
		case 15121000:
		case 31221008:
		case 27121009:
		case 31121004:
		case 33121007:
		case 36121008:
		case 41121005:
		case 42121006:
		case 51121005:
		case 61121014:
		case 65121009:
		case 13121005:
		case 33121004:
			REGISTER_TS(SharpEyes, (pSkillLVLData->m_nX << 8) + pSkillLVLData->m_nCriticaldamageMax);
			break;
		case 5321054:
			REGISTER_TS(AttackCountX, pSkillLVLData->m_nX);
			break;
		case 51111007:
			//ret.monsterStatus.put(MonsterStatus.STUN, Integer.valueOf(1);
			break;
		case 61111002:
			//REGISTER_TS(SUMMON, Integer.valueOf(1);
			//ret.monsterStatus.put(MonsterStatus.STUN, Integer.valueOf(1);
			break;
			//       case 36121002: // TEST HYPO
			//        case 36121013: // TEST HYPO
			//       case 36121014: // TEST HYPO
		case 42100010:
		case 22171052:
		case 42101021:
		case 42121021:
		case 42101001:
			//REGISTER_TS(SUMMON, Integer.valueOf(1);
			break;
		case 42111003:
			
			//REGISTER_TS(SUMMON, Integer.valueOf(1);
			break;
		case 33111006:
		case 33111007:
			REGISTER_TS(Speed, pSkillLVLData->m_nZ);
			REGISTER_TS(BeastFormDamageUp, pSkillLVLData->m_nY);
			REGISTER_TS(BeastFormMaxHP, pSkillLVLData->m_nX);
			REGISTER_TS(IndieBooster, pSkillLVLData->m_nIndieBooster);
			break;
		case 13121004:// Touch of the Wind
			//REGISTER_TS(TOUCH_OF_THE_WIND2, pSkillLVLData->m_nX);
			REGISTER_TS(IllusionStep, pSkillLVLData->m_nY);
			//REGISTER_TS(TOUCH_OF_THE_WIND1, pSkillLVLData->m_nprop);
			REGISTER_TS(IndieMHPR, pSkillLVLData->m_nIndieMhpR);
			break;
		case 31101003:
			REGISTER_TS(GUARD, pSkillLVLData->m_nY);
			break;
		case 51121006:
			REGISTER_TS(DamR, pSkillLVLData->m_nX);
			REGISTER_TS(HowlingCritical, pSkillLVLData->m_nY);
			REGISTER_TS(HowlingCritical, pSkillLVLData->m_nZ);
			break;
		case 51111003:
		{
			REGISTER_TS(DamR, pSkillLVLData->m_nX);
			break;
		}
		case 31121007:
		{
			REGISTER_TS(InfinityForce, 1);
			break;
		}
		case 31111004:
		{
			REGISTER_TS(AsrR, pSkillLVLData->m_nY);
			REGISTER_TS(TerR, pSkillLVLData->m_nZ);
			//REGISTER_TS(DEFENCE_BOOST_R, pSkillLVLData->m_nX);
			break;
		}
		case 400041007: 
		{
			//REGISTER_TS(SECONDARY_STAT_IndieInvincible, 0);
			REGISTER_TS(FlipTheCoin, -1); 
		}
		break;
	}
	pUser->ValidateStat();
	long long int liFlag = 0;
	liFlag |= BasicStat::BasicStatFlag::BS_MaxHP;
	liFlag |= BasicStat::BasicStatFlag::BS_MaxMP;
	if (bResetBySkill)
	{
		pUser->SendTemporaryStatReset(tsFlag);
		ValidateSecondaryStat(pUser);
	}
	else
	{
		pUser->SendTemporaryStatReset(tsFlag);
		pUser->SendTemporaryStatSet(tsFlag, tDelay);
	}
	pUser->SendCharacterStat(true, liFlag);
}

void USkill::DoActiveSkill_WeaponBooster(User* pUser, const SkillEntry * pSkill, int nSLV, int nWT1, int nWT2)
{
}

void USkill::DoActiveSkill_TownPortal(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
}

void USkill::DoActiveSkill_PartyStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
}

void USkill::DoActiveSkill_MobStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, int bSendResult)
{
}

void USkill::DoActiveSkill_Summon(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, bool bResetBySkill, bool bForcedSetTime, int nForcedSetTime)
{
	nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);
	REGISTER_USE_SKILL_SECTION;
	REGISTER_TS(ComboCounter, 1);
	REGISTER_TS(EVAR, 10);
	REGISTER_TS(FireBomb, 10);
	pUser->RemoveSummoned(nSkillID, 0, nSkillID);
	if (bResetBySkill)
	{
		pUser->SendTemporaryStatReset(tsFlag);
		ValidateSecondaryStat(pUser);
	}
	else
	{
		pUser->CreateSummoned(pSkill, nSLV, { pUser->GetPosX(), pUser->GetPosY() }, false);
		pUser->SendTemporaryStatReset(tsFlag);
		pUser->SendTemporaryStatSet(tsFlag, 0);
	}
	pUser->SendCharacterStat(true, 0);
}

void USkill::DoActiveSkill_SmokeShell(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
}

void USkill::ResetTemporaryByTime(User * pUser, const std::vector<int>& aResetReason)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
	for (auto nReason : aResetReason)
	{
		SkillEntry* pSkill = nullptr;
		auto nSLV = SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), nReason, &pSkill, 0, 0, 0, 0);
		if (pSkill)
		{
			USkill::OnSkillUseRequest(
				pUser,
				nullptr,
				pSkill,
				nSLV,
				true,
				false,
				0
			);
		}
	}
}
