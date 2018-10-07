#pragma once
#include "FieldObj.h"
#include <mutex>
#include <condition_variable>
#include <map>
#include <vector>
#include "TemporaryStat.h"

class ClientSocket;
class OutPacket;
class Field;
class Portal;
class InPacket;
class Npc;
struct GA_Character;

class BasicStat;
class SecondaryStat;
struct TemporaryStat;
struct AttackInfo;
struct ActItem;

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

	};

private:
	std::mutex m_mtxUserlock, m_scriptLock;
	int m_nCharacterID;
	ClientSocket *m_pSocket;
	GA_Character *m_pCharacterData;
	BasicStat* m_pBasicStat;
	SecondaryStat* m_pSecondaryStat;
	void *m_pUpdateTimer;
	Script* m_pScript = nullptr;
	std::condition_variable m_cvForScript;
	TransferStatus m_nTransferStatus;

	void TryParsingDamageData(AttackInfo *pInfo, InPacket *iPacket);
	AttackInfo* TryParsingMeleeAttack(int nType, InPacket *iPacket);
	AttackInfo* TryParsingMagicAttack(int nType, InPacket *iPacket);
	AttackInfo* TryParsingShootAttack(int nType, InPacket *iPacket);
	AttackInfo* TryParsingAreaDot(int nType, InPacket *iPacket);
	AttackInfo* TryParsingBodyAttack(int nType, InPacket *iPacket);

	void OnIssueReloginCookie(InPacket* iPacket);

public:

	static User* FindUser(int nUserID);

	User(ClientSocket *pSocket, InPacket *iPacket);
	~User();

	//Basic Routine
	int GetUserID() const;
	int GetChannelID() const;
	std::mutex& GetLock();
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
	void OnTransferChannelRequest(InPacket* iPacket);
	void OnMigrateToCashShopRequest(InPacket* iPacket);
	void OnChat(InPacket *iPacket);
	void OnAttack(int nType, InPacket *iPacket);
	void OnLevelUp();
	void PostTransferField(int dwFieldID, Portal* pPortal, int bForce);
	void SetMovePosition(int x, int y, char bMoveAction, short nFSN);

	//Avatar
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

	//Script
	Script* GetScript();
	void SetScript(Script* pScript);
	void OnSelectNpc(InPacket *iPacket);
	void OnScriptMessageAnswer(InPacket *iPacket);
	void OnScriptRun();
	void OnScriptDone();

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
};

