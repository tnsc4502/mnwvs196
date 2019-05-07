#pragma once
#include <atomic>
#include <mutex>
#include <set>
#include <map>

struct GW_ItemSlotBase;
struct GW_ItemSlotEquip;
struct GW_ItemSlotBundle;
struct GW_CharacterStat;
struct GW_CharacterLevel;
struct GW_CharacterMoney;
struct GW_CharacterSlotCount;
struct GW_SkillRecord;
struct GW_QuestRecord;
struct GW_Avatar;

class InPacket;
class OutPacket;

struct GA_Character
{
private:
	typedef long long int ATOMIC_COUNT_TYPE;
	static ATOMIC_COUNT_TYPE InitCharacterID();

	static ATOMIC_COUNT_TYPE IncCharacterID()
	{
		static std::atomic<ATOMIC_COUNT_TYPE> atomicCharacterID = InitCharacterID();
		return ++atomicCharacterID;
	}

	std::mutex mCharacterLock;

#ifdef DBLIB
	void LoadItemSlot();
	void LoadSkillRecord();
	void LoadQuestRecord();
#endif

public:

	int nWorldID, nAccountID, nGuildID, nPartyID, nFieldID, nCharacterID;
	std::string strName;

	GW_Avatar *mAvatarData = nullptr;
	GW_CharacterStat *mStat = nullptr;
	GW_CharacterLevel *mLevel = nullptr;
	GW_CharacterMoney *mMoney = nullptr;
	GW_CharacterSlotCount *mSlotCount = nullptr;

	std::map<int, GW_SkillRecord*> mSkillRecord;
	std::map<int, GW_QuestRecord*> mQuestRecord;
	std::map<int, GW_QuestRecord*> mQuestComplete;
	std::map<int, GW_ItemSlotBase*> mItemSlot[6];

	std::map<int, int> mItemTrading[6];

	//For recording the liItemSN of the item which was dropped or removed.
	std::set<long long int> mItemRemovedRecord[6];

	void Load(int nCharacterID);
	void LoadAvatar(int nCharacterID);
	void Save(bool isNewCharacter = false);
	void SaveInventoryRemovedRecord();

	void DecodeStat(InPacket *iPacket);
	void DecodeCharacterData(InPacket *iPacket, bool bForInternal);
	void DecodeInventoryData(InPacket *iPacket, bool bForInternal);
	void DecodeInventoryRemovedRecord(InPacket *iPacket);

	void DecodeAvatarLook(InPacket* iPacket);
	void DecodeSkillRecord(InPacket* iPacket);

	void EncodeCharacterData(OutPacket *oPacket, bool bForInternal);
	void EncodeInventoryData(OutPacket *oPacket, bool bForInternal);
	void EncodeInventoryRemovedRecord(OutPacket *oPacket);

	void EncodeAvatar(OutPacket *oPacket);
	void EncodeAvatarLook(OutPacket *oPacket);
	void EncodeStat(OutPacket *oPacket);
	void EncodeSkillRecord(OutPacket *oPacket);

	GA_Character();
	~GA_Character();

	//GW_ItemSlot
	GW_ItemSlotBase* GetItem(int nTI, int nPOS);
	GW_ItemSlotBase* GetItemByID(int nItemID);
	int FindEmptySlotPosition(int nTI);
	void RemoveItem(int nTI, int nPOS);
	int FindCashItemSlotPosition(int nTI, long long int liSN);
	int FindGeneralItemSlotPosition(int nTI, int nItemID, long long int dateExpire, long long int liSN);
	int GetEmptySlotCount(int nTI);
	int GetItemCount(int nTI, int nItemID);
	void SetItem(int nTI, int nPOS, GW_ItemSlotBase* pItem);
	bool IsWearing(int nEquipItemID);

	//GW_SkillRecord
	decltype(mSkillRecord)& GetCharacterSkillRecord();
	GW_SkillRecord* GetSkill(int nSkillID);
	void ObtainSkillRecord(GW_SkillRecord* pRecord);

	void SetQuest(int nKey, const std::string& sInfo);
	void RemoveQuest(int nKey);

	std::mutex& GetCharacterDatLock();
};