#pragma once
#include "FieldObj.h"
#include <mutex>
#include <map>
#include <vector>
#include "TemporaryStat.h"

class ClientSocket;
class OutPacket;
class Field;
class Portal;
class InPacket;
class Npc;
class Pet;
class AsyncScheduler;
struct GA_Character;
struct GW_Avatar;
struct GW_FuncKeyMapped;

class BasicStat;
class SecondaryStat;
struct TemporaryStat;
struct AttackInfo;
struct ActItem;
class SkillEntry;
class Summoned;

class Script;

class User : public FieldObj
{
public:

	//User正在進行的轉換狀態
	//TransferChannel與TransferShop 要附帶SecondaryStat::EncodeInternal以便Center做轉傳
	enum class TransferStatus : unsigned char
	{
		eOnTransferNone = 0x00, //正常 
		eOnTransferField = 0x01, //更換地圖中
		eOnTransferChannel = 0x02, //更換頻道中
		eOnTransferShop = 0x03, //進入商城中
	};

	enum class Message : unsigned char
	{
		eDropPickUpMessage = 0x00,
		eQuestRecordMessage = 0x01,
		eQuestRecordMessageAddValidCheck = 0x02,
		eCashItemExpireMessage = 0x03,
		eIncEXPMessage = 0x04,
		eIncSPMessage = 0x05,
		eIncPOPMessage = 0x06,
		eIncMoneyMessage = 0x07,
		eIncGPMessage = 0x08,
		eIncCommitmentMessage = 0x09,
		eGiveBuffMessage = 0x0A,
		eGeneralItemExpireMessage = 0x0B,
		eSystemMessage = 0x0C,
		eQuestRecordExMessage = 0x0D,
		eWorldShareRecordMessage = 0x0E,
		eItemProtectExpireMessage = 0x0F,
		eItemExpireReplaceMessage = 0x10,
		eItemAbilityTimeLimitedExpireMessage = 0x11,
		eSkillExpireMessage = 0x12,
		eIncNonCombatStatEXPMessage = 0x13,
		eLimitNonCombatStatEXPMessage = 0x14,
		//0x15
		eAndroidMachineHeartAlsetMessage = 0x16,
		eIncFatigueByRestMessage = 0x17,
		eIncPvPPointMessage = 0x18,
		ePvPItemUseMessage = 0x19,
		eWeddingPortalError = 0x1A,
		eIncHardCoreExpMessage = 0x1B,
		eNoticeAutoLineChanged = 0x1C,
		eEntryRecordMessage = 0x1D,
		eEvolvingSystemMessage = 0x1E,
		eEvolvingSystemMessageWithName = 0x1F,
		eCoreInvenOperationMessage = 0x20,
		eNxRecordMessage = 0x21,
		eBlockedBehaviorMessage = 0x22,
		eIncWPMessage = 0x23,
		eMaxWPMessage = 0x24,
		eStylishKillMessage = 0x25,
		eExpiredCashItemResultMessage = 0x26,
		eCollectionRecordMessage = 0x27,
		//0x28
		eQuestExpired = 0x29,
		eWriteMessageInGame = 0x2A,
		eFishSystemBonus = 0x2B,
		eWriteGreenMessageInGame = 0x2C
	};

	enum Effect : unsigned char
	{
		eEffect_LevelUp = 0x00, //Flag only

		/*
		Encode4(nSkillID)
		Encode1(nSLV)
		Encode4(nSelectRoot) = "affect%d"
		Encode4(nSelect) = "affect0\%d"
		*/
		eEffect_ShowSkillAffected = 0x05,

		/*
		Encode1(nType)
		Encode4(nPetPOS)
		*/
		eEffect_PetEffect = 0x09,
		eEffect_ResistPVP = 0x0A, //Flag only

		//nItemType 1 = 護身符 2 = 復仇女神的紡車 4 = 戰鬥機器人的力量 
		//nItemType > 4 : 使用了 ... 未喪失經驗值
		eEffect_ShowItemUsedMessage = 0x0C,

		eEffect_PlayChangeFieldSound = 0x0E, //Flag only
		eEffect_ChangeJobEffect = 0x0F,//Flag only
		eEffect_QuestCompleteEffect = 0x010, //Flag only

		//Encode4(nVal)
		eEffect_IncDecHpEffect = 0x11,

		//Encode4(nVal)
		//Enocde1(bGuard)
		eEffect_IncDecHpEffectGuard = 0x25,

		//Encode4(nItemID)
		eEffect_BuffItemUsed = 0x12,

		//nItemID : 4
		//bEffect : 1
		//if(bEffect) sEffect : str  //"Effect/BasicEff/Event1/Success"
		eEffect_ShowEffectOnItemUsed = 0x15,

		eEffect_SpecialLevelUpEffect = 0x16, //Flag only
		
		//nGain : 4
		eEffect_GainMesoEffect = 0x18,

		//nGain : 4
		eEffect_GainEXPEffect = 0x1A,
		eEffect_SoulStoneReviveMessage = 0x24, //Flag only

		//nRemain : 1
		eEffect_ReviveItemRemainMessage = 0x1C,
		eEffect_OnChargingEffect = 0x29, //Flag only
		eEffect_OnMissEffect = 0x2A, //Flag only
		eEffect_OnCoolEffect = 0x2B, //Flag only
		eEffect_OnGoodEffect = 0x2C,  //Flag only

		//sEffect : str //"Effect/Direction4.img/effect/cannonshooter/balloon/0"
		//Encode1(1)
		//nPeriodInMS : 4
		//nRangeType : 4 (1~4)
		eEffect_OnParametrizeEffect = 0x2F,
		eEffect_OnGradeUpEffect = 0x32, //Flag only

		//持續閃爍
		eEffect_HitPeriodRemainRevive = 0x33, //Flag only

		//nMobSkillID : 4
		//nMobSkillSLV : 4
		eEffect_MobSkillEffect = 0x36,
		eEffect_AswanDefenceFail = 0x37,

		//nItemID : 4
		//nCount : 4
		eEffect_HitBossShieldEffect = 0x39,

		//nResult : 1
		eEffect_JewelryCraftingResult = 0x3B,

		//nVal : 4
		//tDelay : 4
		eEffect_DelayIncDecHpEffect = 0x4F
	};

private:
	static const int MAX_PET_INDEX = 3;

	std::recursive_mutex m_mtxUserlock, m_scriptLock;
	int m_tLastUpdateTime = 0;
	ClientSocket *m_pSocket;
	GA_Character *m_pCharacterData;
	GW_FuncKeyMapped *m_pFuncKeyMapped;
	BasicStat* m_pBasicStat;
	SecondaryStat* m_pSecondaryStat;
	AsyncScheduler *m_pUpdateTimer;
	Script* m_pScript = nullptr;
	TransferStatus m_nTransferStatus;
	Npc *m_pTradingNpc = nullptr;
	Pet* m_apPet[MAX_PET_INDEX] = { nullptr };
	std::vector<Summoned*> m_lSummoned;
	std::vector<int> m_aMigrateSummoned;

	void TryParsingDamageData(AttackInfo *pInfo, InPacket *iPacket);
	AttackInfo* TryParsingMeleeAttack(AttackInfo* pInfo, int nType, InPacket *iPacket);
	AttackInfo* TryParsingMagicAttack(AttackInfo* pInfo, int nType, InPacket *iPacket);
	AttackInfo* TryParsingShootAttack(AttackInfo* pInfo, int nType, InPacket *iPacket);
	AttackInfo* TryParsingAreaDot(AttackInfo* pInfo, int nType, InPacket *iPacket);
	AttackInfo* TryParsingBodyAttack(AttackInfo* pInfo, int nType, InPacket *iPacket);

	void OnIssueReloginCookie(InPacket* iPacket);

public:

	static User* FindUser(int nUserID);

	User(ClientSocket *pSocket, InPacket *iPacket);
	~User();

	//Basic Routine
	int GetUserID() const;
	int GetChannelID() const;
	std::recursive_mutex& GetLock();
	void Update();

	GA_Character* GetCharacterData();
	Field* GetField();
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void SendPacket(OutPacket *oPacket);
	void OnPacket(InPacket *iPacket);
	void LeaveField();
	void OnMigrateIn();
	void OnMigrateOut();

	//TransferStatus
	void SetTransferStatus(TransferStatus e);
	TransferStatus GetTransferStatus() const;

	void OnTransferFieldRequest(InPacket* iPacket);
	bool TryTransferField(int nFieldID, const std::string& sPortalName);
	void OnTransferChannelRequest(InPacket* iPacket);
	void OnMigrateToCashShopRequest(InPacket* iPacket);
	void OnChat(InPacket *iPacket);
	void EncodeChatMessage(OutPacket *oPacket, const std::string strMsg, bool bAdmin, bool bBallon);
	void OnAttack(int nType, InPacket *iPacket);
	void ResetOnStateForOnOffSkill(AttackInfo *pAttackInfo);
	void OnLevelUp();
	void PostTransferField(int dwFieldID, Portal* pPortal, int bForce);
	void SetMovePosition(int x, int y, char bMoveAction, short nFSN);

	//Avatar
private:
	void UpdateAvatar();

public:
	GW_Avatar* GetAvatar();
	void OnAvatarModified();
	void EncodeCharacterData(OutPacket *oPacket);
	void EncodeCoupleInfo(OutPacket *oPacket);
	void EncodeFriendshipInfo(OutPacket *oPacket);
	void EncodeMarriageInfo(OutPacket *oPacket);

	//Stat
	SecondaryStat* GetSecondaryStat();
	BasicStat* GetBasicStat();
	void ValidateStat(bool bCalledByConstructor = false);
	void SendCharacterStat(bool bOnExclRequest, long long int liFlag);
	void SendTemporaryStatReset(TemporaryStat::TS_Flag& flag);
	void SendTemporaryStatSet(TemporaryStat::TS_Flag& flag, int tDelay);
	void ResetTemporaryStat(int tCur, int nReasonID);

	void OnStatChangeItemUseRequest(InPacket *iPacket, bool bByPet);
	void OnStatChangeItemCancelRequest(InPacket *iPacket);

	//Message
	void SendDropPickUpResultPacket(bool bPickedUp, bool bIsMoney, int nItemID, int nCount, bool bOnExcelRequest);
	void SendDropPickUpFailPacket(bool bOnExcelRequest);
	void SendQuestRecordMessage(int nKey, int nState, const std::string& sStringRecord);

	//Npc & Script
	Script* GetScript();
	void SetScript(Script* pScript);
	void OnSelectNpc(InPacket *iPacket);
	void OnScriptMessageAnswer(InPacket *iPacket);
	void SetTradingNpc(Npc *pNpc);
	Npc* GetTradingNpc();

	//Quest
	void OnQuestRequest(InPacket *iPacket);
	void OnAcceptQuest(InPacket *iPacket, int nQuestID, int dwTemplateID, Npc *pNpc);
	void OnCompleteQuest(InPacket *iPacket, int nQuestID, int dwTemplateID, Npc *pNpc, bool bIsAutoComplete);
	void OnResignQuest(InPacket *iPacket, int nQuestID);
	void OnLostQuestItem(InPacket *iPacket, int nQuestID);
	void TryQuestStartAct(int nQuestID, Npc *pNpc);
	void TryQuestCompleteAct(int nQuestID, Npc *pNpc);
	void TryExchange(const std::vector<ActItem*>& aActItem);
	bool AllowToGetQuestItem(const ActItem* pActionItem);

	void SendQuestResult(int nResult, int nQuestID, int dwTemplateID);

	//Message
	void SendChatMessage(int nType, const std::string& sMsg);
	void SendNoticeMessage(int nType, const std::string& sMsg);

	//Func Key Mapped
	void SendFuncKeyMapped();
	void OnFuncKeyMappedModified(InPacket *iPacket);

	//Pet
	void OnPetPacket(InPacket *iPacket);
	void ActivatePet(int nPos, int nRemoveReaseon, bool bOnInitialize);
	int GetMaxPetIndex();
	void OnActivatePetRequest(InPacket *iPacket);

	//Summoned
	void OnSummonedPacket(InPacket *iPacket);
	void ReregisterSummoned();
	void CreateSummoned(const SkillEntry* pSkill, int nSLV, const FieldPoint& pt, bool bMigrate);
	void RemoveSummoned(int nSkillID, int nLeaveType, int nForceRemoveSkillID); //nForceRemoveSkillID = -1 means that remove all summoneds.
};

