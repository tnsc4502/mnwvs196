#pragma once
#include "FieldObj.h"
#include <mutex>
#include <map>
#include "TemporaryStat.h"

class ClientSocket;
class OutPacket;
class Field;
class Portal;
class InPacket;
struct GA_Character;

class BasicStat;
class SecondaryStat;
struct TemporaryStat;
struct AttackInfo;

class User : public FieldObj
{

private:
	std::mutex m_mtxUserlock;
	int nCharacterID;
	ClientSocket *pSocket;
	GA_Character *pCharacterData;
	BasicStat* m_pBasicStat;
	SecondaryStat* m_pSecondaryStat;
	void *m_pUpdateTimer;

	void TryParsingDamageData(AttackInfo *pInfo, InPacket *iPacket);
	AttackInfo* TryParsingMeleeAttack(int nType, InPacket *iPacket);
	AttackInfo* TryParsingMagicAttack(int nType, InPacket *iPacket);
	AttackInfo* TryParsingShootAttack(int nType, InPacket *iPacket);
	AttackInfo* TryParsingAreaDot(int nType, InPacket *iPacket);
	AttackInfo* TryParsingBodyAttack(int nType, InPacket *iPacket);

	void OnIssueReloginCookie(InPacket* iPacket);

public:
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

	User() {}
	User(ClientSocket *pSocket, InPacket *iPacket);
	~User();

	int GetUserID() const;

	GA_Character* GetCharacterData();
	Field* GetField();
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void SendPacket(OutPacket *oPacket);
	void OnPacket(InPacket *iPacket);
	void LeaveField();

	void OnTransferFieldRequest(InPacket* iPacket);
	void OnChat(InPacket *iPacket);
	void PostTransferField(int dwFieldID, Portal* pPortal, int bForce);
	void SetMovePosition(int x, int y, char bMoveAction, short nFSN);

	void OnAvatarModified();
	void EncodeCoupleInfo(OutPacket *oPacket);
	void EncodeFriendshipInfo(OutPacket *oPacket);
	void EncodeMarriageInfo(OutPacket *oPacket);

	void ValidateStat();
	void SendCharacterStat(bool bOnExclRequest, long long int liFlag);
	void SendTemporaryStatReset(TemporaryStat::TS_Flag& flag);
	void SendTemporaryStatSet(TemporaryStat::TS_Flag& flag, int tDelay);
	void OnAttack(int nType, InPacket *iPacket);
	void OnLevelUp();

	SecondaryStat* GetSecondaryStat();
	BasicStat* GetBasicStat();

	std::mutex& GetLock();
	void Update();
	void ResetTemporaryStat(int tCur, int nReasonID);

	void MigrateOut();
	
	static User* FindUser(int nUserID);

	void SendDropPickUpResultPacket(bool bPickedUp, bool bIsMoney, int nItemID, int nCount, bool bOnExcelRequest);
	void SendDropPickUpFailPacket(bool bOnExcelRequest);
};

