#pragma once
#include "GW_ItemSlotEquip.h"
#include "GW_ItemSlotBundle.h"
#include "GW_CharacterStat.h"
#include "GW_CharacterLevel.h"
#include "GW_CharacterMoney.h"
#include "GW_Avatar.hpp"

#include <atomic>

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

	std::vector<GW_ItemSlotEquip> aEquipItem;
	std::vector<GW_ItemSlotBundle> aCONItem, aETCItem, aINSItem;
	//Cash Item...

	void Encode(OutPacket *oPacket);
	void EncodeAvatar(OutPacket *oPacket);
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
};