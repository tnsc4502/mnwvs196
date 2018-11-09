#pragma once
#include <vector>

class User;
class OutPacket;
class InPacket;
class SkillEntry;

class USkill
{
	static void ValidateSecondaryStat(User *pUser);
public:
	 USkill() = delete;
	~USkill() = delete;

	static void OnSkillUseRequest(User* pUser, InPacket *iPacket, const SkillEntry* pEntry, int nSLV, bool bResetBySkill, bool bForceSetTime, int nForceSetTime);
	static void OnSkillUseRequest(User* pUser, InPacket *iPacket);
	static void OnSkillUpRequest(User* pUser, InPacket *iPacket);
	static void OnSkillUpRequest(User* pUser, int nSkillID, int nAmount, bool bDecSP, bool bCheckMasterLevel);

	static void OnSkillPrepareRequest(User* pUser, InPacket *iPacket);
	static void OnSkillCancelRequest(User* pUser, InPacket *iPacket);
	static void SendFailPacket(User* pUser);
	static void DoActiveSkill_SelfStatChange(User* pUser, const SkillEntry *pSkill, int nSLV, InPacket *iPacket, int nOptionValue, bool bResetBySkill, bool bForcedSetTime = false, int nForcedSetTime = 0);
	static void DoActiveSkill_WeaponBooster(User* pUser, const SkillEntry *pSkill, int nSLV, int nWT1, int nWT2);
	static void DoActiveSkill_TownPortal(User* pUser, const SkillEntry *pSkill, int nSLV, InPacket *iPacket);
	static void DoActiveSkill_PartyStatChange(User* pUser, const SkillEntry *pSkill, int nSLV, InPacket *iPacket);
	static void DoActiveSkill_MobStatChange(User* pUser, const SkillEntry *pSkill, int nSLV, InPacket *iPacket, int bSendResult);
	static void DoActiveSkill_Summon(User* pUser, const SkillEntry *pSkill, int nSLV, InPacket *iPacket, bool bResetBySkill, bool bForcedSetTime = false, int nForcedSetTime = 0);
	static void DoActiveSkill_SmokeShell(User* pUser, const SkillEntry *pSkill, int nSLV, InPacket *iPacket);
	static void ResetTemporaryByTime(User* pUser, const std::vector<int>& aResetReason);

};

