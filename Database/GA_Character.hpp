#pragma once
#include "GW_ItemSlotEquip.h"
#include "GW_ItemSlotBundle.h"
#include "GW_CharacterStat.h"
#include "GW_CharacterLevel.h"
#include "GW_CharacterMoney.h"
#include "GW_Avatar.hpp"

#include <atomic>
#include <map>

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

public:

	unsigned char nGender;
	int nWorldID, nAccountID, nFame, nGuildID, nPartyID, nFieldID, nCharacterID;
	std::string strName;

	GW_Avatar *mAvatarData = nullptr;
	GW_CharacterStat *mStat = nullptr;
	GW_CharacterLevel *mLevel = nullptr;
	GW_CharacterMoney *mMoney = nullptr;

	std::map<short, GW_ItemSlotBase*> mItemSlot[6];
	//std::vector<GW_ItemSlotBundle> aCONItem, aETCItem, aINSItem;
	//Cash Item...

	void DecodeCharacterData(InPacket *iPacket);
	void DecodeStat(InPacket *iPacket);
	void DecodeInventoryData(InPacket *iPacket);

	void EncodeCharacterData(OutPacket *oPacket);
	void EncodeInventoryData(OutPacket *oPacket);

	void EncodeAvatar(OutPacket *oPacket);
	void EncodeAvatarLook(OutPacket *oPacket);
	void EncodeStat(OutPacket *oPacket);

	GA_Character();
	~GA_Character()
	{
		delete mAvatarData;
		delete mStat;
		delete mLevel;
		delete mMoney;
	}

	void Load(int nCharacterID);
	void LoadAvatar(int nCharacterID);
	void Save(bool isNewCharacter = false);

	int FindEmptySlotPosition(int nTI);
	GW_ItemSlotBase* GetItem(int nTI, int nPOS);
	int FindCashItemSlotPosition(int nTI, long long int liSN);
	int FindGeneralItemSlotPosition(int nTI, int nItemID, long long int dateExpire, long long int liSN);
	int GetEmptySlotCount(int nTI);
	int GetItemCount(int nTI, int nItemID);
	void SetItem(int nTI, int nPOS, GW_ItemSlotBase* pItem);
};