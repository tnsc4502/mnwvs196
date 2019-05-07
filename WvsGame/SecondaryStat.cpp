#include "SecondaryStat.h"
#include "BasicStat.h"
#include "ItemInfo.h"
#include "KaiserSkills.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "USkill.h"
#include "User.h"
#include "Field.h"
#include "LifePool.h"
#include "ForceAtom.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_CharacterLevel.h"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"

#include "..\WvsLib\Logger\WvsLogger.h"

#define CHECK_TS_NORMAL(name) \
if (flag & GET_TS_FLAG(name)) \
{ if (EnDecode4Byte(flag))oPacket->Encode4(n##name); else oPacket->Encode2(n##name);\
oPacket->Encode4(r##name); oPacket->Encode4(t##name); }


SecondaryStat::SecondaryStat()
{
}


SecondaryStat::~SecondaryStat()
{
}

void SecondaryStat::SetFrom(GA_Character * pChar, BasicStat * pBS)
{
	const GW_CharacterStat *pCS = pChar->mStat;

	this->nPAD = 0;
	this->nPDD = 0;

	//不知道新的計算公式為何
	this->nMAD = pBS->nINT;
	this->nMDD = pBS->nINT;

	this->nEVA = pBS->nLUK / 2 + pBS->nDEX / 4;
	this->nACC = pBS->nDEX + pBS->nLUK;
	this->nSpeed = 100;
	this->nJump = 100;
	this->nCraft = pBS->nDEX + pBS->nLUK + pBS->nINT;

	int nPDDIncRate = 0; //shield mastery ?

	const GW_ItemSlotEquip* pEquip;
	for (const auto& itemEquipped : pChar->mItemSlot[1])
	{
		pEquip = (const GW_ItemSlotEquip*)itemEquipped.second;

		nPDD += pEquip->nPDD;
		nPAD += pEquip->nPAD;
		nMDD += pEquip->nMDD;
		nMAD += pEquip->nMAD;

		nACC += pEquip->nACC;
		nEVA += pEquip->nEVA;
	}
	SkillEntry
		*pShoutOfEmpress = nullptr,
		*pMichaelShoutOfEmpress = nullptr,
		*pUltimateAdventurer = nullptr,
		*pReinforcementOfEmpress = nullptr;

	int nCheckSLV = 0;
	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 10000074, &pShoutOfEmpress, 0, 0, 0, 1);
	if (nCheckSLV && pShoutOfEmpress)
	{
		auto pLevelData = pShoutOfEmpress->GetLevelData(nCheckSLV);
		nIncMaxHPr = pLevelData->m_nX;
		nIncMaxMPr = pLevelData->m_nX;
	}

	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 50000074, &pMichaelShoutOfEmpress, 0, 0, 0, 1);
	if (nCheckSLV && pMichaelShoutOfEmpress)
	{
		auto pLevelData = pMichaelShoutOfEmpress->GetLevelData(nCheckSLV);
		nIncMaxHPr = pLevelData->m_nX;
		nIncMaxMPr = pLevelData->m_nX;
	}

	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 74, &pUltimateAdventurer, 0, 0, 0, 1);
	if (nCheckSLV && pUltimateAdventurer)
	{
		auto pLevelData = pUltimateAdventurer->GetLevelData(nCheckSLV);
		nLevel = pLevelData->m_nX;
	}

	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 80, &pReinforcementOfEmpress, 0, 0, 0, 1);
	if (nCheckSLV && pReinforcementOfEmpress)
	{
		auto pLevelData = pReinforcementOfEmpress->GetLevelData(nCheckSLV);
		nLevel = pLevelData->m_nX;
	}

	nIncMaxHPr_Forced = nIncMaxHPr;
}

void SecondaryStat::EncodeForLocal(OutPacket * oPacket, TemporaryStat::TS_Flag & flag)
{
	flag.Encode(oPacket);
	CHECK_TS_NORMAL(STR);
	CHECK_TS_NORMAL(INT);
	CHECK_TS_NORMAL(DEX);
	CHECK_TS_NORMAL(LUK);
	CHECK_TS_NORMAL(PAD);
	CHECK_TS_NORMAL(PDD);
	CHECK_TS_NORMAL(MAD);
	CHECK_TS_NORMAL(MDD);
	CHECK_TS_NORMAL(ACC);
	CHECK_TS_NORMAL(EVA);
	CHECK_TS_NORMAL(EVAR);
	CHECK_TS_NORMAL(Craft);
	CHECK_TS_NORMAL(Speed);
	CHECK_TS_NORMAL(Jump);
	CHECK_TS_NORMAL(EMHP);
	CHECK_TS_NORMAL(EMMP);
	CHECK_TS_NORMAL(EPAD);
	CHECK_TS_NORMAL(EMAD);
	CHECK_TS_NORMAL(EPDD);
	CHECK_TS_NORMAL(EMDD);
	CHECK_TS_NORMAL(MagicGuard);
	CHECK_TS_NORMAL(DarkSight);
	CHECK_TS_NORMAL(Booster);
	CHECK_TS_NORMAL(PowerGuard);
	//WRITE_TS_NORMAL(Guard);
	CHECK_TS_NORMAL(MaxHP);
	CHECK_TS_NORMAL(MaxMP);
	CHECK_TS_NORMAL(Invincible);
	CHECK_TS_NORMAL(SoulArrow);
	CHECK_TS_NORMAL(Stun);
	CHECK_TS_NORMAL(Shock);
	CHECK_TS_NORMAL(Poison);
	CHECK_TS_NORMAL(Seal);
	CHECK_TS_NORMAL(Darkness);
	CHECK_TS_NORMAL(ComboCounter);
	CHECK_TS_NORMAL(WeaponCharge);
	CHECK_TS_NORMAL(ElementalCharge);
	CHECK_TS_NORMAL(HolySymbol);
	CHECK_TS_NORMAL(MesoUp);
	CHECK_TS_NORMAL(ShadowPartner);
	CHECK_TS_NORMAL(PickPocket);
	CHECK_TS_NORMAL(MesoGuard);
	CHECK_TS_NORMAL(Thaw);
	CHECK_TS_NORMAL(Weakness);
	CHECK_TS_NORMAL(WeaknessMdamage);
	CHECK_TS_NORMAL(Curse);
	CHECK_TS_NORMAL(Slow);
	CHECK_TS_NORMAL(TimeBomb);
	CHECK_TS_NORMAL(BuffLimit);
	CHECK_TS_NORMAL(Team);
	CHECK_TS_NORMAL(DisOrder);
	CHECK_TS_NORMAL(Thread);
	CHECK_TS_NORMAL(Morph);
	CHECK_TS_NORMAL(Ghost);
	CHECK_TS_NORMAL(Regen);
	CHECK_TS_NORMAL(BasicStatUp);
	CHECK_TS_NORMAL(Stance);
	CHECK_TS_NORMAL(SharpEyes);
	CHECK_TS_NORMAL(ManaReflection);
	CHECK_TS_NORMAL(Attract);
	CHECK_TS_NORMAL(Magnet);
	CHECK_TS_NORMAL(MagnetArea);
	CHECK_TS_NORMAL(NoBulletConsume);
	CHECK_TS_NORMAL(StackBuff);
	CHECK_TS_NORMAL(Trinity);
	CHECK_TS_NORMAL(Infinity);
	CHECK_TS_NORMAL(AdvancedBless);
	CHECK_TS_NORMAL(IllusionStep);
	CHECK_TS_NORMAL(Blind);
	CHECK_TS_NORMAL(Concentration);
	CHECK_TS_NORMAL(BanMap);
	CHECK_TS_NORMAL(MaxLevelBuff);
	CHECK_TS_NORMAL(Barrier);
	CHECK_TS_NORMAL(DojangShield);
	CHECK_TS_NORMAL(ReverseInput);
	CHECK_TS_NORMAL(MesoUpByItem);
	CHECK_TS_NORMAL(ItemUpByItem);
	CHECK_TS_NORMAL(RespectPImmune);
	CHECK_TS_NORMAL(RespectMImmune);
	CHECK_TS_NORMAL(DefenseAtt);
	CHECK_TS_NORMAL(DefenseState);
	CHECK_TS_NORMAL(DojangBerserk);
	CHECK_TS_NORMAL(DojangInvincible);
	CHECK_TS_NORMAL(SoulMasterFinal);
	CHECK_TS_NORMAL(WindBreakerFinal);
	CHECK_TS_NORMAL(ElementalReset);
	CHECK_TS_NORMAL(HideAttack);
	CHECK_TS_NORMAL(EventRate);
	CHECK_TS_NORMAL(ComboAbilityBuff);
	CHECK_TS_NORMAL(ComboDrain);
	CHECK_TS_NORMAL(ComboBarrier);
	CHECK_TS_NORMAL(PartyBarrier);
	CHECK_TS_NORMAL(BodyPressure);
	CHECK_TS_NORMAL(RepeatEffect);
	CHECK_TS_NORMAL(ExpBuffRate);
	CHECK_TS_NORMAL(StopPortion);
	CHECK_TS_NORMAL(StopMotion);
	CHECK_TS_NORMAL(Fear);
	CHECK_TS_NORMAL(MagicShield);
	CHECK_TS_NORMAL(MagicResistance);
	CHECK_TS_NORMAL(SoulStone);
	CHECK_TS_NORMAL(Flying);
	CHECK_TS_NORMAL(NewFlying);
	CHECK_TS_NORMAL(NaviFlying);
	CHECK_TS_NORMAL(Frozen);
	CHECK_TS_NORMAL(Frozen2);
	CHECK_TS_NORMAL(Web);
	CHECK_TS_NORMAL(Enrage);
	CHECK_TS_NORMAL(NotDamaged);
	CHECK_TS_NORMAL(FinalCut);
	CHECK_TS_NORMAL(HowlingAttackDamage);
	CHECK_TS_NORMAL(BeastFormDamageUp);
	CHECK_TS_NORMAL(Dance);
	CHECK_TS_NORMAL(Cyclone);
	CHECK_TS_NORMAL(OnCapsule);
	CHECK_TS_NORMAL(HowlingCritical);
	CHECK_TS_NORMAL(HowlingMaxMP);
	CHECK_TS_NORMAL(HowlingDefence);
	CHECK_TS_NORMAL(HowlingEvasion);
	CHECK_TS_NORMAL(Conversion);
	CHECK_TS_NORMAL(Revive);
	CHECK_TS_NORMAL(PinkbeanMinibeenMove);
	CHECK_TS_NORMAL(Sneak);
	CHECK_TS_NORMAL(Mechanic);
	CHECK_TS_NORMAL(DrawBack);
	CHECK_TS_NORMAL(BeastFormMaxHP);
	CHECK_TS_NORMAL(Dice);
	CHECK_TS_NORMAL(BlessingArmor);
	CHECK_TS_NORMAL(BlessingArmorIncPAD);
	CHECK_TS_NORMAL(DamR);
	CHECK_TS_NORMAL(TeleportMasteryOn);
	CHECK_TS_NORMAL(CombatOrders);
	CHECK_TS_NORMAL(Beholder);
	CHECK_TS_NORMAL(DispelItemOption);
	CHECK_TS_NORMAL(DispelItemOptionByField);
	CHECK_TS_NORMAL(Inflation);
	//WRITE_TS_NORMAL(OnixDivineProtection);
	CHECK_TS_NORMAL(Bless);
	CHECK_TS_NORMAL(Explosion);
	CHECK_TS_NORMAL(DarkTornado);
	CHECK_TS_NORMAL(IncMaxHP);
	CHECK_TS_NORMAL(IncMaxMP);
	CHECK_TS_NORMAL(PVPDamage);
	CHECK_TS_NORMAL(PVPDamageSkill);
	CHECK_TS_NORMAL(PvPScoreBonus);
	CHECK_TS_NORMAL(PvPInvincible);
	CHECK_TS_NORMAL(PvPRaceEffect);
	CHECK_TS_NORMAL(IceKnight);
	CHECK_TS_NORMAL(HolyMagicShell);
	CHECK_TS_NORMAL(InfinityForce);
	CHECK_TS_NORMAL(AmplifyDamage);
	CHECK_TS_NORMAL(KeyDownTimeIgnore);
	CHECK_TS_NORMAL(MasterMagicOn);
	CHECK_TS_NORMAL(AsrR);
	CHECK_TS_NORMAL(AsrRByItem);
	CHECK_TS_NORMAL(TerR);
	CHECK_TS_NORMAL(DamAbsorbShield);
	CHECK_TS_NORMAL(Roulette);
	CHECK_TS_NORMAL(Event);
	CHECK_TS_NORMAL(SpiritLink);
	CHECK_TS_NORMAL(CriticalBuff);
	CHECK_TS_NORMAL(DropRate);
	CHECK_TS_NORMAL(PlusExpRate);
	CHECK_TS_NORMAL(ItemInvincible);
	CHECK_TS_NORMAL(ItemCritical);
	CHECK_TS_NORMAL(ItemEvade);
	CHECK_TS_NORMAL(Event2);
	CHECK_TS_NORMAL(VampiricTouch);
	CHECK_TS_NORMAL(DDR);
	CHECK_TS_NORMAL(IncCriticalDamMin);
	CHECK_TS_NORMAL(IncCriticalDamMax);
	CHECK_TS_NORMAL(IncTerR);
	CHECK_TS_NORMAL(IncAsrR);
	CHECK_TS_NORMAL(DeathMark);
	CHECK_TS_NORMAL(PainMark);
	CHECK_TS_NORMAL(UsefulAdvancedBless);
	CHECK_TS_NORMAL(Lapidification);
	CHECK_TS_NORMAL(VampDeath);
	CHECK_TS_NORMAL(VampDeathSummon);
	CHECK_TS_NORMAL(VenomSnake);
	CHECK_TS_NORMAL(CarnivalAttack);
	CHECK_TS_NORMAL(CarnivalDefence);
	CHECK_TS_NORMAL(CarnivalExp);
	CHECK_TS_NORMAL(SlowAttack);
	CHECK_TS_NORMAL(PyramidEffect);
	CHECK_TS_NORMAL(HollowPointBullet);
	CHECK_TS_NORMAL(KeyDownMoving);
	CHECK_TS_NORMAL(KeyDownAreaMoving);
	CHECK_TS_NORMAL(CygnusElementSkill);
	CHECK_TS_NORMAL(IgnoreTargetDEF);
	CHECK_TS_NORMAL(Invisible);
	CHECK_TS_NORMAL(ReviveOnce);
	CHECK_TS_NORMAL(AntiMagicShell);
	CHECK_TS_NORMAL(EnrageCr);
	CHECK_TS_NORMAL(EnrageCrDamMin);
	CHECK_TS_NORMAL(BlessOfDarkness);
	CHECK_TS_NORMAL(LifeTidal);
	CHECK_TS_NORMAL(Judgement);
	CHECK_TS_NORMAL(DojangLuckyBonus);
	CHECK_TS_NORMAL(HitCriDamR);
	CHECK_TS_NORMAL(Larkness);
	CHECK_TS_NORMAL(SmashStack);
	CHECK_TS_NORMAL(ReshuffleSwitch);
	CHECK_TS_NORMAL(SpecialAction);
	CHECK_TS_NORMAL(ArcaneAim);
	CHECK_TS_NORMAL(StopForceAtomInfo);
	CHECK_TS_NORMAL(SoulGazeCriDamR);
	CHECK_TS_NORMAL(SoulRageCount);
	CHECK_TS_NORMAL(PowerTransferGauge);
	CHECK_TS_NORMAL(AffinitySlug);
	CHECK_TS_NORMAL(SoulExalt);
	CHECK_TS_NORMAL(HiddenPieceOn);
	CHECK_TS_NORMAL(IncMaxDamage);
	CHECK_TS_NORMAL(BossShield);
	CHECK_TS_NORMAL(MobZoneState);
	CHECK_TS_NORMAL(GiveMeHeal);
	CHECK_TS_NORMAL(TouchMe);
	CHECK_TS_NORMAL(Contagion);
	CHECK_TS_NORMAL(ComboUnlimited);
	CHECK_TS_NORMAL(IgnorePCounter);
	CHECK_TS_NORMAL(IgnoreAllCounter);
	CHECK_TS_NORMAL(IgnorePImmune);
	CHECK_TS_NORMAL(IgnoreAllImmune);
	CHECK_TS_NORMAL(FinalJudgement);
	CHECK_TS_NORMAL(KnightsAura);
	CHECK_TS_NORMAL(IceAura);
	CHECK_TS_NORMAL(FireAura);
	CHECK_TS_NORMAL(VengeanceOfAngel);
	CHECK_TS_NORMAL(HeavensDoor);
	CHECK_TS_NORMAL(Preparation);
	CHECK_TS_NORMAL(BullsEye);
	CHECK_TS_NORMAL(IncEffectHPPotion);
	CHECK_TS_NORMAL(IncEffectMPPotion);
	CHECK_TS_NORMAL(SoulMP);
	CHECK_TS_NORMAL(FullSoulMP);
	CHECK_TS_NORMAL(SoulSkillDamageUp);
	CHECK_TS_NORMAL(BleedingToxin);
	CHECK_TS_NORMAL(IgnoreMobDamR);
	CHECK_TS_NORMAL(Asura);
	CHECK_TS_NORMAL(FlipTheCoin);
	CHECK_TS_NORMAL(UnityOfPower);
	CHECK_TS_NORMAL(Stimulate);
	CHECK_TS_NORMAL(ReturnTeleport);
	CHECK_TS_NORMAL(CapDebuff);
	CHECK_TS_NORMAL(DropRIncrease);
	CHECK_TS_NORMAL(IgnoreMobpdpR);
	CHECK_TS_NORMAL(BDR);
	CHECK_TS_NORMAL(Exceed);
	CHECK_TS_NORMAL(DiabolikRecovery);
	CHECK_TS_NORMAL(FinalAttackProp);
	CHECK_TS_NORMAL(ExceedOverload);
	CHECK_TS_NORMAL(DevilishPower);
	CHECK_TS_NORMAL(OverloadCount);
	CHECK_TS_NORMAL(BuckShot);
	CHECK_TS_NORMAL(FireBomb);
	CHECK_TS_NORMAL(HalfstatByDebuff);
	CHECK_TS_NORMAL(SurplusSupply);
	CHECK_TS_NORMAL(SetBaseDamage);
	CHECK_TS_NORMAL(AmaranthGenerator);
	CHECK_TS_NORMAL(StrikerHyperElectric);
	CHECK_TS_NORMAL(EventPointAbsorb);
	CHECK_TS_NORMAL(EventAssemble);
	CHECK_TS_NORMAL(StormBringer);
	CHECK_TS_NORMAL(ACCR);
	CHECK_TS_NORMAL(DEXR);
	CHECK_TS_NORMAL(Albatross);
	CHECK_TS_NORMAL(Translucence);
	CHECK_TS_NORMAL(PoseType);
	CHECK_TS_NORMAL(LightOfSpirit);
	CHECK_TS_NORMAL(ElementSoul);
	CHECK_TS_NORMAL(GlimmeringTime);
	CHECK_TS_NORMAL(Restoration);
	CHECK_TS_NORMAL(ComboCostInc);
	CHECK_TS_NORMAL(ChargeBuff);
	CHECK_TS_NORMAL(TrueSight);
	CHECK_TS_NORMAL(CrossOverChain);
	CHECK_TS_NORMAL(ChillingStep);
	CHECK_TS_NORMAL(Reincarnation);
	CHECK_TS_NORMAL(DotBasedBuff);
	CHECK_TS_NORMAL(BlessEnsenble);
	CHECK_TS_NORMAL(ExtremeArchery);
	CHECK_TS_NORMAL(QuiverCatridge);
	CHECK_TS_NORMAL(AdvancedQuiver);
	CHECK_TS_NORMAL(UserControlMob);
	CHECK_TS_NORMAL(ImmuneBarrier);
	CHECK_TS_NORMAL(ArmorPiercing);
	CHECK_TS_NORMAL(ZeroAuraStr);
	CHECK_TS_NORMAL(ZeroAuraSpd);
	CHECK_TS_NORMAL(CriticalGrowing);
	CHECK_TS_NORMAL(QuickDraw);
	CHECK_TS_NORMAL(BowMasterConcentration);
	CHECK_TS_NORMAL(TimeFastABuff);
	CHECK_TS_NORMAL(TimeFastBBuff);
	CHECK_TS_NORMAL(GatherDropR);
	CHECK_TS_NORMAL(AimBox2D);
	CHECK_TS_NORMAL(CursorSniping);
	CHECK_TS_NORMAL(IncMonsterBattleCaptureRate);
	CHECK_TS_NORMAL(DebuffTolerance);
	CHECK_TS_NORMAL(DotHealHPPerSecond);
	CHECK_TS_NORMAL(SpiritGuard);
	CHECK_TS_NORMAL(PreReviveOnce);
	CHECK_TS_NORMAL(SetBaseDamageByBuff);
	CHECK_TS_NORMAL(LimitMP);
	CHECK_TS_NORMAL(ReflectDamR);
	CHECK_TS_NORMAL(ComboTempest);
	CHECK_TS_NORMAL(MHPCutR);
	CHECK_TS_NORMAL(MMPCutR);
	CHECK_TS_NORMAL(SelfWeakness);
	CHECK_TS_NORMAL(ElementDarkness);
	CHECK_TS_NORMAL(FlareTrick);
	CHECK_TS_NORMAL(Ember);
	CHECK_TS_NORMAL(Dominion);
	CHECK_TS_NORMAL(SiphonVitality);
	CHECK_TS_NORMAL(DarknessAscension);
	CHECK_TS_NORMAL(BossWaitingLinesBuff);
	CHECK_TS_NORMAL(DamageReduce);
	CHECK_TS_NORMAL(ShadowServant);
	CHECK_TS_NORMAL(ShadowIllusion);
	CHECK_TS_NORMAL(AddAttackCount);
	CHECK_TS_NORMAL(ComplusionSlant);
	CHECK_TS_NORMAL(JaguarSummoned);
	CHECK_TS_NORMAL(JaguarCount);
	CHECK_TS_NORMAL(SSFShootingAttack);
	CHECK_TS_NORMAL(DevilCry);
	CHECK_TS_NORMAL(ShieldAttack);
	CHECK_TS_NORMAL(BMageAura);
	CHECK_TS_NORMAL(DarkLighting);
	CHECK_TS_NORMAL(AttackCountX);
	CHECK_TS_NORMAL(BMageDeath);
	CHECK_TS_NORMAL(BombTime);
	CHECK_TS_NORMAL(NoDebuff);
	CHECK_TS_NORMAL(XenonAegisSystem);
	CHECK_TS_NORMAL(AngelicBursterSoulSeeker);
	CHECK_TS_NORMAL(HiddenPossession);
	CHECK_TS_NORMAL(NightWalkerBat);
	CHECK_TS_NORMAL(NightLordMark);
	CHECK_TS_NORMAL(WizardIgnite);
	CHECK_TS_NORMAL(BattlePvP_Helena_Mark);
	CHECK_TS_NORMAL(BattlePvP_Helena_WindSpirit);
	CHECK_TS_NORMAL(BattlePvP_LangE_Protection);
	CHECK_TS_NORMAL(BattlePvP_LeeMalNyun_ScaleUp);
	CHECK_TS_NORMAL(BattlePvP_Revive);
	CHECK_TS_NORMAL(PinkbeanAttackBuff);
	CHECK_TS_NORMAL(RandAreaAttack);
	CHECK_TS_NORMAL(BattlePvP_Mike_Shield);
	CHECK_TS_NORMAL(BattlePvP_Mike_Bugle);
	CHECK_TS_NORMAL(PinkbeanRelax);
	CHECK_TS_NORMAL(PinkbeanYoYoStack);
	CHECK_TS_NORMAL(NextAttackEnhance);
	CHECK_TS_NORMAL(AranBeyonderDamAbsorb);
	CHECK_TS_NORMAL(AranCombotempastOption);
	CHECK_TS_NORMAL(NautilusFinalAttack);
	CHECK_TS_NORMAL(ViperTimeLeap);
	CHECK_TS_NORMAL(RoyalGuardState);
	CHECK_TS_NORMAL(RoyalGuardPrepare);
	CHECK_TS_NORMAL(MichaelSoulLink);
	CHECK_TS_NORMAL(MichaelStanceLink);
	CHECK_TS_NORMAL(TriflingWhimOnOff);
	CHECK_TS_NORMAL(AddRangeOnOff);
	CHECK_TS_NORMAL(KinesisPsychicPoint);
	CHECK_TS_NORMAL(KinesisPsychicOver);
	CHECK_TS_NORMAL(KinesisPsychicShield);
	CHECK_TS_NORMAL(KinesisIncMastery);
	CHECK_TS_NORMAL(KinesisPsychicEnergeShield);
	CHECK_TS_NORMAL(BladeStance);
	CHECK_TS_NORMAL(DebuffActiveSkillHPCon);
	CHECK_TS_NORMAL(DebuffIncHP);
	CHECK_TS_NORMAL(BowMasterMortalBlow);
	CHECK_TS_NORMAL(AngelicBursterSoulResonance);
	CHECK_TS_NORMAL(Fever);
	CHECK_TS_NORMAL(IgnisRore);
	CHECK_TS_NORMAL(RpSiksin);
	CHECK_TS_NORMAL(TeleportMasteryRange);
	CHECK_TS_NORMAL(FireBarrier);
	CHECK_TS_NORMAL(ChangeFoxMan);
	CHECK_TS_NORMAL(FixCoolTime);
	CHECK_TS_NORMAL(IncMobRateDummy);
	CHECK_TS_NORMAL(AdrenalinBoost);
	CHECK_TS_NORMAL(AranSmashSwing);
	CHECK_TS_NORMAL(AranDrain);
	CHECK_TS_NORMAL(AranBoostEndHunt);
	CHECK_TS_NORMAL(HiddenHyperLinkMaximization);
	CHECK_TS_NORMAL(RWCylinder);
	CHECK_TS_NORMAL(RWCombination);
	CHECK_TS_NORMAL(RWMagnumBlow);
	CHECK_TS_NORMAL(RWBarrier);
	CHECK_TS_NORMAL(RWBarrierHeal);
	CHECK_TS_NORMAL(RWMaximizeCannon);
	CHECK_TS_NORMAL(RWOverHeat);
	CHECK_TS_NORMAL(RWMovingEvar);
	CHECK_TS_NORMAL(Stigma);

	if (flag & GET_TS_FLAG(SoulMP))
	{
		oPacket->Encode4(xSoulMP);
		oPacket->Encode4(rSoulMP);
	}

	if (flag & GET_TS_FLAG(FullSoulMP))
	{
		oPacket->Encode4(xFullSoulMP);
		oPacket->Encode4(rFullSoulMP);
	}

	int nCount = 0;
	oPacket->Encode2(nCount);
	for (int i = 0; i < nCount; ++i)
	{
		oPacket->Encode4(0); //mBuffedForSpecMap
		oPacket->Encode1(0); //bEnable
	}
	oPacket->Encode1((int)nDefenseAtt);
	oPacket->Encode1((int)nDefenseState);
	oPacket->Encode1((int)nPVPDamage);

	if (flag & GET_TS_FLAG(Dice))
	{
		for (int i = 0; i < 22; ++i)
			oPacket->Encode4(0);
	}

	if (flag & GET_TS_FLAG(KillingPoint))
		oPacket->Encode1((int)nKillingPoint);

	if (flag & GET_TS_FLAG(PinkbeanRollingGrade))
		oPacket->Encode1((int)nPinkbeanRollingGrade);

	if (flag & GET_TS_FLAG(Judgement))
		oPacket->Encode4(xJudgement);

	if (flag & GET_TS_FLAG(StackBuff))
		oPacket->Encode1((int)mStackBuff);

	if (flag & GET_TS_FLAG(PinkbeanRollingGrade))
		oPacket->Encode1((int)mTrinity);

	if (flag & GET_TS_FLAG(ElementalCharge))
	{
		oPacket->Encode1((int)mElementalCharge);
		oPacket->Encode2(wElementalCharge);
		oPacket->Encode1((int)uElementalCharge);
		oPacket->Encode1((int)zElementalCharge);
	}

	if (flag & GET_TS_FLAG(LifeTidal))
		oPacket->Encode4((int)nLifeTidal); //for mLifeTidal

	if (flag & GET_TS_FLAG(AntiMagicShell))
		oPacket->Encode1((int)bAntiMagicShell);

	if (flag & GET_TS_FLAG(Larkness))
	{
		//LarknessInfo::Encode
		for (int i = 0; i < 2; ++i)
		{
			oPacket->Encode4(0);
			oPacket->Encode4(0);
		}
		oPacket->Encode4(dgLarkness);
		oPacket->Encode4(lgLarkness);
	}

	if (flag & GET_TS_FLAG(IgnoreTargetDEF))
		oPacket->Encode1((int)bIgnoreTargetDEF);

	if (flag & GET_TS_FLAG(StopForceAtomInfo))
		sStopForceAtomInfo.Encode(oPacket);

	if (flag & GET_TS_FLAG(SmashStack))
		oPacket->Encode4(xSmashStack);

	if (flag & GET_TS_FLAG(MobZoneState))
		oPacket->Encode4(0);


	if (flag & GET_TS_FLAG(Slow))
		oPacket->Encode1((int)bSlow);

	if (flag & GET_TS_FLAG(IceAura))
		oPacket->Encode1((int)bIceAura);

	if (flag & GET_TS_FLAG(KnightsAura))
		oPacket->Encode1((int)bKnightsAura);

	if (flag & GET_TS_FLAG(IgnoreMobpdpR))
		oPacket->Encode1((int)bIgnoreMobpdpR);

	if (flag & GET_TS_FLAG(BDR))
		oPacket->Encode1((int)bBDR);

	if (flag & GET_TS_FLAG(DropRIncrease))
	{
		oPacket->Encode4(xDropRIncrease);
		oPacket->Encode1((int)bDropRIncrease);
	}

	if (flag & GET_TS_FLAG(PoseType))
		oPacket->Encode1((int)bPoseType);

	if (flag & GET_TS_FLAG(Beholder))
	{
		oPacket->Encode4(sBeholder);
		oPacket->Encode4(ssBeholder);
	}

	if (flag & GET_TS_FLAG(CrossOverChain))
		oPacket->Encode4(xCrossOverChain); //for xCrossOverChain

	if (flag & GET_TS_FLAG(Reincarnation))
		oPacket->Encode4(xReincarnation); //for xReincarnation

	if (flag & GET_TS_FLAG(ExtremeArchery))
	{
		oPacket->Encode4(bExtremeArchery);
		oPacket->Encode4(xExtremeArchery);
	}

	if (flag & GET_TS_FLAG(QuiverCatridge))
		oPacket->Encode4(xQuiverCatridge); 
	if (flag & GET_TS_FLAG(ImmuneBarrier))
		oPacket->Encode4(xImmuneBarrier);

	if (flag & GET_TS_FLAG(ZeroAuraStr))
		oPacket->Encode1((int)bZeroAuraStr);

	if (flag & GET_TS_FLAG(ZeroAuraSpd))
		oPacket->Encode1((int)bZeroAuraSpd);

	if (flag & GET_TS_FLAG(ArmorPiercing))
		oPacket->Encode4(bArmorPiercing); 

	if (flag & GET_TS_FLAG(SharpEyes))
		oPacket->Encode4(mSharpEyes);
	if (flag & GET_TS_FLAG(AdvancedBless))
		oPacket->Encode4(xAdvancedBless);
	if (flag & GET_TS_FLAG(DotHealHPPerSecond))
		oPacket->Encode4(xDotHealHPPerSecond);
	if (flag & GET_TS_FLAG(SpiritGuard))
		oPacket->Encode4(nSpiritGuard);

	if (flag & GET_TS_FLAG(KnockBack))
	{
		oPacket->Encode4(nKnockBack);
		oPacket->Encode4(bKnockBack);
	}

	if (flag & GET_TS_FLAG(ShieldAttack))
		oPacket->Encode4(xShieldAttack);

	if (flag & GET_TS_FLAG(SSFShootingAttack))
		oPacket->Encode4(xSSFShootingAttack);

	if (flag & GET_TS_FLAG(BMageAura)) 
	{
		oPacket->Encode4(xBMageAura);
		oPacket->Encode1((int)bBMageAura);
	}

	if (flag & GET_TS_FLAG(BattlePvP_Helena_Mark))
		oPacket->Encode4(cBattlePvP_Helena_Mark);
	if (flag & GET_TS_FLAG(PinkbeanAttackBuff))
		oPacket->Encode4(bPinkbeanAttackBuff);

	if (flag & GET_TS_FLAG(RoyalGuardState))
	{
		oPacket->Encode4(bRoyalGuardState);
		oPacket->Encode4(xRoyalGuardState);
	}
	if (flag & GET_TS_FLAG(MichaelSoulLink))
	{
		oPacket->Encode4(xMichaelSoulLink);
		oPacket->Encode1((int)bMichaelSoulLink);
		oPacket->Encode4(cMichaelSoulLink);
		oPacket->Encode4(yMichaelSoulLink);
	}

	if (flag & GET_TS_FLAG(AdrenalinBoost))
		oPacket->Encode1((int)cAdrenalinBoost);

	if (flag & GET_TS_FLAG(RWCylinder))
	{
		oPacket->Encode1((int)bRWCylinder);
		oPacket->Encode2(cRWCylinder);
	}

	if (flag & GET_TS_FLAG(RWMagnumBlow))
	{
		oPacket->Encode2(bRWMagnumBlow);
		oPacket->Encode1(xRWMagnumBlow);
	}

	//ViperEnergyCharge
	oPacket->Encode4(0);

	if (flag & GET_TS_FLAG(BladeStance))
		oPacket->Encode4(xBladeStance);

	if (flag & GET_TS_FLAG(DarkSight))
		oPacket->Encode4(cDarkSight);

	if (flag & GET_TS_FLAG(Stigma))
		oPacket->Encode4(bStigma);

	for (int i = 0; i < 8; ++i)
	{
		if (flag & TemporaryStat::TS_Flag(TemporaryStat::TS_EnergyCharged + i))
		{
			oPacket->Encode4(0);
			oPacket->Encode4(0);
			oPacket->Encode1(0);
			oPacket->Encode4(0);
		}
	}
	EncodeIndieTempStat(oPacket, flag);
	//EecodeIndieTempStat

	if (flag & GET_TS_FLAG(UsingScouter))
		oPacket->Encode4(nUsingScouter);

	oPacket->Encode2(1);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "Encode Local TS : \n");
	oPacket->Print();
}

void SecondaryStat::EncodeForRemote(OutPacket * oPacket, TemporaryStat::TS_Flag & flag)
{
	flag.Encode(oPacket);

	oPacket->Encode1((char)nDefenseAtt);
	oPacket->Encode1((char)nDefenseState);
	oPacket->Encode1((char)nPVPDamage);

	sStopForceAtomInfo.Encode(oPacket);

	oPacket->Encode4(0); //nViperCharge

	//RideVechicle
}

void SecondaryStat::EncodeIndieTempStat(OutPacket * oPacket, TemporaryStat::TS_Flag & flag)
{
	int nValue, rValue, tValue = 0;
	bool bValid = false;
	for (int i = 0; i <= TemporaryStat::TS_INDIE_STAT_COUNT; ++i)
	{
		if (flag & TemporaryStat::TS_Flag(i))
		{
			auto& mIndieTS = m_mSetByIndieTS[i];
			oPacket->Encode4((int)mIndieTS.size());
			for (auto& indieTS : mIndieTS)
			{
				bValid = indieTS.second.second.size() != 0;
				nValue = bValid ? (indieTS.second.second[0]) : 0;
				rValue = bValid ? (indieTS.second.second[1]) : 0;
				tValue = bValid ? (indieTS.second.second[2]) : 0;
				oPacket->Encode4(rValue);
				oPacket->Encode4(nValue);
				oPacket->Encode4(INT_MAX);
				oPacket->Encode4(1);
				oPacket->Encode4(tValue);
				oPacket->Encode4(0);
			}
		}
	}
}

bool SecondaryStat::EnDecode4Byte(TemporaryStat::TS_Flag & flag)
{
	if ((flag & GET_TS_FLAG(CarnivalDefence))
		|| (flag & GET_TS_FLAG(SpiritLink))
		|| (flag & GET_TS_FLAG(DojangLuckyBonus))
		|| (flag & GET_TS_FLAG(SoulGazeCriDamR))
		|| (flag & GET_TS_FLAG(PowerTransferGauge))
		|| (flag & GET_TS_FLAG(ReturnTeleport))
		|| (flag & GET_TS_FLAG(ShadowPartner))
		|| (flag & GET_TS_FLAG(SetBaseDamage))
		|| (flag & GET_TS_FLAG(QuiverCatridge))
		|| (flag & GET_TS_FLAG(ImmuneBarrier))
		|| (flag & GET_TS_FLAG(NaviFlying))
		|| (flag & GET_TS_FLAG(Dance))
		|| (flag & GET_TS_FLAG(SetBaseDamageByBuff))
		|| (flag & GET_TS_FLAG(DotHealHPPerSecond))
		|| (flag & GET_TS_FLAG(MagnetArea))
		|| (flag & GET_TS_FLAG(RideVehicle)))
		return true;
	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "EnDecode4Byte [False]\n");
	return false;
}

void SecondaryStat::ResetByTime(User* pUser, int tCur)
{
	std::vector<int> aSkillResetReason;
	auto pSS = pUser->GetSecondaryStat();
	for (auto& setFlag : pSS->m_mSetByTS)
	{
		int nID = *(setFlag.second.second[1]);
		int tValue = *(setFlag.second.second[2]);
		if (!((tCur - setFlag.second.first) > tValue))
			continue;
		if (nID < 0)
		{
			auto pItemInfo = ItemInfo::GetInstance()->GetStateChangeItem(-nID);
			if (pItemInfo)
				pUser->SendTemporaryStatReset(pItemInfo->Apply(pUser, 0, false, true));
		}
		else 
			aSkillResetReason.push_back(nID);
	}

	//IndieTS
	for (auto& mIndieTS : pSS->m_mSetByIndieTS)
	{
		for (auto& indieTS : mIndieTS.second)
		{
			int nID = (indieTS.second.second[1]);
			int tValue = (indieTS.second.second[2]);
			if (!((tCur - indieTS.second.first) > tValue))
				continue;
			if (nID < 0)
			{
				auto pItemInfo = ItemInfo::GetInstance()->GetStateChangeItem(-nID);
				if (pItemInfo)
					pUser->SendTemporaryStatReset(pItemInfo->Apply(pUser, 0, false, true));
			}
			else
				aSkillResetReason.push_back(nID);
		}
	}
	USkill::ResetTemporaryByTime(pUser, aSkillResetReason);
}

void SecondaryStat::DecodeInternal(User* pUser, InPacket * iPacket)
{
	bool bDecodeInternal = iPacket->Decode1() == 1;
	if (!bDecodeInternal)
		return;
	int nChannelID = iPacket->Decode4();

	//Decode Temporary Internal
	int nCount = iPacket->Decode4(), nSkillID, tDurationRemained, nSLV;
	for (int i = 0; i < nCount; ++i)
	{
		nSkillID = iPacket->Decode4();
		tDurationRemained = iPacket->Decode4();
		nSLV = iPacket->Decode4();
		WvsLogger::LogFormat("Decode Internal ID = %d, tValue = %d, nSLV = %d\n", nSkillID, tDurationRemained, nSLV);
		if (nSkillID < 0)
		{
			auto pItem = ItemInfo::GetInstance()->GetStateChangeItem(-nSkillID);
			if (pItem)
				pItem->Apply(pUser, 0, false, false, true, tDurationRemained);
		}
		else
			USkill::OnSkillUseRequest(
				pUser,
				nullptr,
				SkillInfo::GetInstance()->GetSkillByID(nSkillID),
				nSLV,
				false,
				true,
				tDurationRemained
			);
	}
}

void SecondaryStat::EncodeInternal(User* pUser, OutPacket * oPacket)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());

	oPacket->Encode4(pUser->GetChannelID());

	//Encode Temporary Internal
	auto pSS = pUser->GetSecondaryStat();
	oPacket->Encode4((int)pSS->m_mSetByTS.size() + (int)pSS->m_mSetByIndieTS.size());
	for (auto& setFlag : pSS->m_mSetByTS)
	{
		int nSkillID = *(setFlag.second.second[1]);
		int tDurationRemained = (int)setFlag.second.first;
		int nSLV = *(setFlag.second.second[3]);
		oPacket->Encode4(nSkillID);
		oPacket->Encode4(tDurationRemained);
		oPacket->Encode4(nSLV);
	}

	for (auto& mIndieTS : pSS->m_mSetByIndieTS)
	{
		for (auto& setFlag : mIndieTS.second)
		{
			int nSkillID = (setFlag.second.second[1]);
			int tDurationRemained = (int)setFlag.second.first;
			int nSLV = (setFlag.second.second[3]);
			oPacket->Encode4(nSkillID);
			oPacket->Encode4(tDurationRemained);
			oPacket->Encode4(nSLV);
		}
	}
}

//傑諾能量補充
void SecondaryStat::ChargeSurplusSupply(User * pUser, int nCount, int tUpdateTime)
{
	if (tUpdateTime - tSurplusSupply < 4000)
		return;
	int nLevel = pUser->GetCharacterData()->mLevel->nLevel;
	int nMaxAmount = (nLevel >= 100 ? 20 : nLevel >= 60 ? 15 : nLevel >= 30 ? 10 : 5);
	if (nCount > nMaxAmount)
		nCount = nMaxAmount;
	nSurplusSupply += nCount;
	if (nSurplusSupply >= nMaxAmount)
		nSurplusSupply = nMaxAmount;
	auto tsFlag = GET_TS_FLAG(SurplusSupply);

	pUser->SendTemporaryStatReset(tsFlag);
	pUser->SendTemporaryStatSet(tsFlag, -1);
	tSurplusSupply = tUpdateTime;
}

//凱撒龍蛇球補充
void SecondaryStat::ChargeSmashStack(User * pUser, int tUpdateTime)
{
	const int nToCharge = 33, nMaxChargeValue = 1000;
	if (xSmashStack + nToCharge > nMaxChargeValue)
		return;

	nSmashStack += nToCharge;
	xSmashStack += nToCharge;

	auto tsFlag = GET_TS_FLAG(SmashStack);
	pUser->SendTemporaryStatReset(tsFlag);
	pUser->SendTemporaryStatSet(tsFlag, -1);
}

void SecondaryStat::StopForceAtom::CreateStopForceAtom(User * pUser, int nSkillID)
{
	aAngelInfo.clear();

	switch (nSkillID)
	{
		case KaiserSkills::TempestBlades_6110:
			nIdx = 1;
			nCount = 3;
			break;
		case KaiserSkills::TempestBlades_6111_2:
			nIdx = 3;
			nCount = 3;
			break;
		case KaiserSkills::AdvancedTempestBlades_6112:
			nIdx = 2;
			nCount = 5;
			break;
		case KaiserSkills::AdvancedTempestBlades_6112_2:
			nIdx = 4;
			nCount = 5;
			break;
	}
	auto pWeapon = pUser->GetCharacterData()->GetItem(
		GW_ItemSlotBase::EQUIP, -11
	);
	if (pWeapon)
		nWeaponID = pWeapon->nItemID;
}

void SecondaryStat::StopForceAtom::Encode(OutPacket * oPacket)
{
	oPacket->Encode4(nIdx);
	oPacket->Encode4(nCount);
	oPacket->Encode4(nWeaponID);
	for (int i = 0; i < nCount; ++i)
		aAngelInfo.push_back(0);
	//WvsLogger::LogFormat("StopForceAtom nIdx = %d nCount = %d nWeaponID = %d\n", nIdx, nCount, nWeaponID);
	oPacket->Encode4((int)aAngelInfo.size());
	for (const auto& value : aAngelInfo)
		oPacket->Encode4(value);
}

void SecondaryStat::StopForceAtom::OnTempestBladesAttack(User *pUser, InPacket * iPacket)
{
	/*
	：0xE9 0x01 
	0x01 0x00 0x00 0x00 
	0x09 0x10 0x00 0x00
	*/
	if (nCount > 0)
	{
		int nMobCount = iPacket->Decode4(), nMobObjID = 0;
		std::vector<int> aMob;
		for (int i = 0; i < nMobCount; ++i)
		{
			nMobObjID = iPacket->Decode4();
			if (pUser->GetField()->GetLifePool()->GetMob(nMobObjID))
				aMob.push_back(nMobObjID);
		}

		WvsLogger::LogFormat("aMob size() = %d xStopForceAtomInfo = %d nCount =%d\n", (int)aMob.size(), pUser->GetSecondaryStat()->xStopForceAtomInfo, nCount);
		if (aMob.size() > 0)
		{
			ForceAtom atom;
			++(pUser->GetSecondaryStat()->xStopForceAtomInfo);
			atom.CreateForceAtom(
				pUser->GetUserID(),
				pUser->GetSecondaryStat()->rStopForceAtomInfo,
				false,
				true,
				pUser->GetUserID(),
				ForceAtom::ForceAtomType::e_TempestBlade,
				pUser->GetSecondaryStat()->xStopForceAtomInfo,
				nCount,
				pUser->GetField(),
				{ 0, 0 }
			);
			pUser->GetSecondaryStat()->xStopForceAtomInfo += (nCount - 1);

			atom.m_adwTargetMob = aMob;
			atom.OnForceAtomCreated(pUser->GetField());
		}

		USkill::ResetTemporaryByTime(
			pUser,
			{ pUser->GetSecondaryStat()->rStopForceAtomInfo }
		);
	}
}
