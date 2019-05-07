#include "GA_Character.hpp"
#include "WvsUnified.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "GW_ItemSlotEquip.h"
#include "GW_ItemSlotBundle.h"
#include "GW_ItemSlotPet.h"
#include "GW_CharacterStat.h"
#include "GW_CharacterLevel.h"
#include "GW_CharacterMoney.h"
#include "GW_CharacterSlotCount.h"
#include "GW_Avatar.hpp"
#include "GW_SkillRecord.h"
#include "GW_QuestRecord.h"

#include <algorithm>

#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"

GA_Character::GA_Character()
	: mAvatarData(AllocObj(GW_Avatar)),
	  mStat(AllocObj(GW_CharacterStat)),
	  mLevel(AllocObj(GW_CharacterLevel)),
	  mMoney(AllocObj(GW_CharacterMoney)),
	  mSlotCount(AllocObj(GW_CharacterSlotCount))
{
}

GA_Character::~GA_Character()
{
	FreeObj(mAvatarData);
	FreeObj(mStat);
	FreeObj(mLevel);
	FreeObj(mMoney);
	FreeObj(mSlotCount);

	for (auto& slot : mItemSlot)
	{
		for (auto& pItem : slot)
			pItem.second->Release();
	}

	for (auto& skill : mSkillRecord)
		FreeObj(skill.second);

	for (auto& qr : mQuestComplete)
		FreeObj(qr.second);

	for (auto& qr : mQuestRecord)
		FreeObj(qr.second);
}

void GA_Character::Load(int nCharacterID)
{
	LoadAvatar(nCharacterID);
	mMoney->Load(nCharacterID);
	mSlotCount->Load(nCharacterID);
	LoadItemSlot();
	LoadSkillRecord();
	LoadQuestRecord();
}

void GA_Character::LoadAvatar(int nCharacterID)
{
	mAvatarData->Load(nCharacterID);
	mStat->Load(nCharacterID);
	mLevel->Load(nCharacterID);

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM Characters Where CharacterID = " << nCharacterID;
	Poco::Data::RecordSet recordSet(queryStatement);
	queryStatement.execute();
	this->nCharacterID = nCharacterID;
	nAccountID = recordSet["AccountID"];
	nWorldID = recordSet["WorldID"];
	strName = recordSet["CharacterName"].toString();
	nGuildID = recordSet["GuildID"];
	nPartyID = recordSet["PartyID"];
	nFieldID = recordSet["FieldID"];
	//nGender = recordSet["Gender"];
}

void GA_Character::EncodeAvatar(OutPacket *oPacket)
{
	EncodeStat(oPacket);
	EncodeAvatarLook(oPacket);
}

void GA_Character::EncodeStat(OutPacket *oPacket)
{
	oPacket->Encode4(nCharacterID);
	oPacket->Encode4(nCharacterID);
	oPacket->Encode4(nWorldID);
	oPacket->EncodeBuffer(
		(unsigned char*)strName.c_str(), 
		(int)strName.size(),
		15 - (int)strName.size());

	oPacket->Encode1(mStat->nGender);
	oPacket->Encode1(0);
	oPacket->Encode1(mStat->nSkin);
	oPacket->Encode4(mStat->nFace);
	oPacket->Encode4(mStat->nHair);
	oPacket->Encode1((char)0xFF);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(mLevel->nLevel); //LEVEL
	oPacket->Encode2(mStat->nJob);

	oPacket->Encode2(mStat->nStr);
	oPacket->Encode2(mStat->nDex);
	oPacket->Encode2(mStat->nInt);
	oPacket->Encode2(mStat->nLuk);
	oPacket->Encode4(mStat->nHP);
	oPacket->Encode4(mStat->nMaxHP);
	oPacket->Encode4(mStat->nMP);
	oPacket->Encode4(mStat->nMaxMP);

	oPacket->Encode2(mStat->nAP);
	mStat->EncodeExtendSP(oPacket);
	//oPacket->Encode1(0); //SP

	oPacket->Encode8(mStat->nExp); //EXP
	oPacket->Encode4(mStat->nFame);
	oPacket->Encode4(0);
	oPacket->Encode8(0); //Gach EXP
	oPacket->Encode8(GameDateTime::GetCurrentDate()); //
	oPacket->Encode4(nFieldID);
	oPacket->Encode1(0); //Inital Spawn Point
	oPacket->Encode2(0); //Get Subcategory

	if (WvsGameConstants::IsDemonSlayerJob(mStat->nJob) || WvsGameConstants::IsXenonJob(mStat->nJob))
		oPacket->Encode4(mStat->nFaceMark);
	//isDemonSlayer || isXenon || isDemonAvenger Encode4

	oPacket->Encode1(0); //Fatigue
	oPacket->Encode4(0);

	for (int i = 0; i < 6; ++i)
		oPacket->Encode4(0);
	for (int i = 0; i < 6; ++i)
		oPacket->Encode2(0);
	oPacket->Encode1(0);
	oPacket->Encode8(GameDateTime::TIME_PERMANENT);

	oPacket->Encode4(0); //PVP EXP
	oPacket->Encode1(0); //PVP RANK
	oPacket->Encode4(0); //BATTLE POINTS
	oPacket->Encode1(0x05);
	oPacket->Encode1(0x06);
	oPacket->Encode4(0);

	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode8(GameDateTime::GetCurrentDate());
	oPacket->Encode4(0);
	oPacket->Encode1(0);

	for (int i = 0; i < 9; ++i)
	{
		oPacket->Encode4(0);
		oPacket->Encode1(0);
		oPacket->Encode4(0);
	}

	oPacket->Encode8(GameDateTime::GetCurrentDate());
	oPacket->Encode8(GameDateTime::GetCurrentDate());
	oPacket->Encode8(GameDateTime::GetCurrentDate());
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode1(0);

	for (int i = 0; i < 25; ++i)
		oPacket->Encode1(0);

	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
}

void GA_Character::EncodeSkillRecord(OutPacket * oPacket)
{
	oPacket->Encode1(1);
	oPacket->Encode2((short)mSkillRecord.size());
	for (auto& skill : mSkillRecord)
		skill.second->Encode(oPacket);
	oPacket->Encode2(0);
	oPacket->Encode4(0);
}

void GA_Character::EncodeAvatarLook(OutPacket *oPacket)
{
	oPacket->Encode1(mStat->nGender);
	oPacket->Encode1(mAvatarData->nSkin);
	oPacket->Encode4(mAvatarData->nFace);
	oPacket->Encode4(mStat->nJob);
	oPacket->Encode1(0); //Mega?
	oPacket->Encode4(mAvatarData->nHair);

	for (const auto& eqp : mAvatarData->mEquip)
	{
		oPacket->Encode1(eqp.first * -1);
		oPacket->Encode4(eqp.second);
	}
	oPacket->Encode1((char)0xFF);
	for (const auto& eqp : mAvatarData->mUnseenEquip)
	{
		oPacket->Encode1((eqp.first * -1) - 100);
		oPacket->Encode4(eqp.second);
	}
	oPacket->Encode1((char)0xFF);
	oPacket->Encode1((char)0xFF); //totem

	int nCashWeaponID = 0, nWeaponID = 0, nShieldID = 0;
	for (auto pEqup : mAvatarData->mEquip)
		if (pEqup.first == -111)
			nCashWeaponID = pEqup.second;
		else if (pEqup.first == -11)
			nWeaponID = pEqup.second;
		else if (pEqup.first == -10)
			nShieldID = pEqup.second;

	oPacket->Encode4(nCashWeaponID);
	oPacket->Encode4(nWeaponID);
	oPacket->Encode4(nShieldID);

	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);


	if (WvsGameConstants::IsDemonSlayerJob(mStat->nJob) || WvsGameConstants::IsXenonJob(mStat->nJob))
		oPacket->Encode4(mStat->nFaceMark);
	else if (WvsGameConstants::IsZeroJob(mStat->nJob))
		oPacket->Encode1(1);
	else if (WvsGameConstants::IsBeastTamerJob(mStat->nJob))
	{
		oPacket->Encode1(1);
		oPacket->Encode4(0);
		oPacket->Encode1(1);
		oPacket->Encode4(0);
	}
	//isDemonSlayer || isXenon || isDemonAvenger oPacket->Encode4(0);
	//isZero oPacket->Encode1(0)

	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
}

void GA_Character::Save(bool bIsNewCharacter)
{
	if (bIsNewCharacter)
	{
		nCharacterID = (int)IncCharacterID();
		Poco::Data::Statement newRecordStatement(GET_DB_SESSION);
		newRecordStatement << "INSERT INTO Characters(CharacterID, AccountID) VALUES(" << nCharacterID << ", " << nAccountID << ")";
		newRecordStatement.execute();
		newRecordStatement.reset(GET_DB_SESSION);
		//newRecordStatement << "SELECT CharacterID FROM Characters"
	}
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "UPDATE Characters Set "
		<< "WorldID = '" << nWorldID << "', "
		//<< "Gender = '" << nGender << "', "
		<< "CharacterName = '" << strName << "', "
		//<< "Fame = '" << nFame << "', "
		//<< "GuildID = '" << nGuildID << "', "
		//<< "PartyID = '" << nPartyID << "', "
		<< "FieldID = '" << nFieldID << "' WHERE CharacterID = " << nCharacterID;
	queryStatement.execute();
	mAvatarData->Save(nCharacterID, bIsNewCharacter);
	mMoney->Save(nCharacterID, bIsNewCharacter);
	mLevel->Save(nCharacterID, bIsNewCharacter);
	mStat->Save(nCharacterID, bIsNewCharacter);
	mSlotCount->Save(nCharacterID, bIsNewCharacter);

	for (auto& eqp : mItemSlot[1])
		((GW_ItemSlotEquip*)(eqp.second))->Save(nCharacterID);
	for (auto& con : mItemSlot[2])
		((GW_ItemSlotBundle*)(con.second))->Save(nCharacterID);
	for (auto& etc : mItemSlot[3])
		((GW_ItemSlotBundle*)(etc.second))->Save(nCharacterID);
	for (auto& ins : mItemSlot[4])
		((GW_ItemSlotBundle*)(ins.second))->Save(nCharacterID);
	for (auto& ins : mItemSlot[5])
	{
		if(ins.second->bIsPet)
			((GW_ItemSlotPet*)(ins.second))->Save(nCharacterID);
		else
			((GW_ItemSlotBundle*)(ins.second))->Save(nCharacterID);
	}
	SaveInventoryRemovedRecord();

	for (auto& skill : mSkillRecord)
		skill.second->Save();
	for (auto& questRecord : mQuestRecord)
		questRecord.second->Save();
	for (auto& questRecord : mQuestComplete)
		questRecord.second->Save();
}

void GA_Character::SaveInventoryRemovedRecord()
{
	GW_ItemSlotEquip equipRemovedInstance;
	GW_ItemSlotBundle bundleRemovedInstance;
	GW_ItemSlotPet petRemovedInstance;
	equipRemovedInstance.nCharacterID = nCharacterID;
	bundleRemovedInstance.nCharacterID = nCharacterID;
	for (int i = 1; i <= 5; ++i)
	{
		for (const auto& liSN : mItemRemovedRecord[i])
			if (i == 1)
			{
				equipRemovedInstance.liItemSN = liSN * -1;
				equipRemovedInstance.nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
				equipRemovedInstance.Save(nCharacterID);
			}
			else
			{
				bundleRemovedInstance.liItemSN = liSN * -1;
				equipRemovedInstance.nType = (GW_ItemSlotBase::GW_ItemSlotType)(GW_ItemSlotBase::GW_ItemSlotType::EQUIP + (i));
				bundleRemovedInstance.Save(nCharacterID);
			}
	}
}

int GA_Character::FindEmptySlotPosition(int nTI)
{
	if (nTI <= 0 || nTI > 5)
		return 0;
	int nLastIndex = 1;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
	{
		if (slot.first < 0) //skip equipped
			continue;
		if (slot.first > mSlotCount->aSlotCount[nTI])
			return 0;
		if (slot.first > nLastIndex || (slot.first == nLastIndex && slot.second == nullptr))
			return nLastIndex;
		nLastIndex = slot.first + 1;
	}
	return nLastIndex > mSlotCount->aSlotCount[nTI] ? 0 : nLastIndex;
}

GW_ItemSlotBase* GA_Character::GetItem(int nTI, int nPOS)
{
	if (nTI <= 0 || nTI > 5)
		return nullptr;
	auto result = mItemSlot[nTI].find(nPOS);
	if (result == mItemSlot[nTI].end())
		return nullptr;
	return result->second;
}

GW_ItemSlotBase * GA_Character::GetItemByID(int nItemID)
{
	int nTI = nItemID / 1000000;
	if (nTI <= 0 || nTI > 5)
		return nullptr;
	auto itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
	{
		if (slot.first < 0) //skip equipped
			continue;
		if (slot.first >= mSlotCount->aSlotCount[nTI])
			return nullptr;
		if (slot.second->nItemID == nItemID)
			return slot.second;
	}
	return nullptr;
}

void GA_Character::RemoveItem(int nTI, int nPOS)
{
	if (nTI <= 0 || nTI > 5)
		return;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	//int nNewPos = (int)mAtomicRemovedIndexCounter++;
	auto pItem = GetItem(nTI, nPOS);
	if (pItem != nullptr)
	{
		mItemSlot[nTI].erase(pItem->nPOS);
		if (pItem->liItemSN != -1)
			mItemRemovedRecord[nTI].insert(pItem->liItemSN);
		if (pItem->nType == GW_ItemSlotBase::EQUIP)
			FreeObj((GW_ItemSlotEquip*)(pItem));
		else
			FreeObj((GW_ItemSlotBundle*)(pItem));
	}
}

int GA_Character::FindCashItemSlotPosition(int nTI, long long int liSN)
{
	if (nTI <= 0 || nTI > 5)
		return 0;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
		if (slot.second->liCashItemSN == liSN)
			return slot.second->nPOS;
	return 0;
}

int GA_Character::FindGeneralItemSlotPosition(int nTI, int nItemID, long long int dateExpire, long long int liSN)
{
	if (nTI <= 0 || nTI > 5)
		return 0;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
		if (slot.second->nItemID == nItemID && !CompareFileTime((FILETIME*)&dateExpire, (FILETIME*)(slot.second->liExpireDate)))
			return slot.second->nPOS;
	return 0;
}

int GA_Character::GetEmptySlotCount(int nTI)
{
	int nCount = 0;
	if (nTI <= 0 || nTI > 5)
		return 0;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[nTI];
	int nLastIndeex = 0;
	for (auto& slot : itemSlot)
	{
		if (slot.second == nullptr)
			++nCount;
		nCount += (slot.first - nLastIndeex - 1);
		nLastIndeex = slot.first;
	}
	return nCount;
}

int GA_Character::GetItemCount(int nTI, int nItemID)
{
	int nCount = 0;
	if (nTI <= 0 || nTI > 5)
		return 0;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
		if (slot.second != nullptr && slot.second->nItemID == nItemID)
			++nCount;
	return nCount;
}

void GA_Character::SetItem(int nTI, int nPOS, GW_ItemSlotBase * pItem)
{
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	if (nTI >= 1 && nTI <= 5)
		mItemSlot[nTI][nPOS] = pItem;
}

bool GA_Character::IsWearing(int nEquipItemID)
{
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[1];
	for (auto& slot : itemSlot)
		if (slot.second != nullptr
			&& slot.second->nItemID == nEquipItemID
			&& slot.second->nPOS < 0)
			return true;
		else if (slot.second->nPOS >= 0)
			return false;
	return false;
}

decltype(GA_Character::mSkillRecord)& GA_Character::GetCharacterSkillRecord()
{
	return mSkillRecord;
}

GW_SkillRecord * GA_Character::GetSkill(int nSkillID)
{
	auto findResult = mSkillRecord.find(nSkillID);
	return findResult == mSkillRecord.end() ? nullptr : findResult->second;
}

void GA_Character::ObtainSkillRecord(GW_SkillRecord * pRecord)
{
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	if (pRecord != nullptr)
		mSkillRecord.insert({ pRecord->nSkillID, pRecord });
}

void GA_Character::SetQuest(int nKey, const std::string & sInfo)
{
	auto findIter = mQuestRecord.find(nKey);
	if (findIter == mQuestRecord.end())
	{
		GW_QuestRecord *pNewRecord = AllocObj(GW_QuestRecord);
		pNewRecord->nState = 1;
		pNewRecord->nQuestID = nKey;
		pNewRecord->sStringRecord = sInfo;
		mQuestRecord.insert({ nKey, pNewRecord });
	}
	else
		findIter->second->sStringRecord = sInfo;
}

void GA_Character::RemoveQuest(int nKey)
{
	mQuestRecord.erase(nKey);
}

std::mutex & GA_Character::GetCharacterDatLock()
{
	return mCharacterLock;
}

void GA_Character::DecodeCharacterData(InPacket *iPacket, bool bForInternal)
{
	long long int flag = iPacket->Decode8();
	iPacket->Decode1();
	for (int i = 0; i < 3; ++i)
		iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode4();
	iPacket->Decode1();

	if (flag & 1)
	{
		DecodeStat(iPacket);
		int nBuddyCapicity = iPacket->Decode1(); //BUDDY LIST CAPACITY

		bool hasBlesOfFairyOrigin = iPacket->Decode1() == 1; //BLESS OF FAIRY ORIGIN
		bool hasBlesOfEmpressOrigin = iPacket->Decode1() == 1; //BLESS OF EMPRESS ORIGIN
		bool hasUltraExplorer = iPacket->Decode1() == 1; //ULTRA EXPLORER

		iPacket->Decode8(); //TIME CURRENT TIME
		iPacket->Decode8(); //TIME -2
		iPacket->Decode4();
		iPacket->Decode1();
		iPacket->Decode4();
		iPacket->Decode1();
	}

	if (flag & 2)
	{
		mMoney->nMoney = iPacket->Decode8();
		nCharacterID = iPacket->Decode4();
		iPacket->Decode4();
		iPacket->Decode4();
	}

	if (flag & 8)
		iPacket->Decode4();

	if (flag & 0x80)
	{
		iPacket->Decode4();
		iPacket->Decode4();
		for (int i = 0; i < 3; ++i)
		{
			iPacket->Decode4();
			iPacket->Decode4();
		}
		iPacket->Decode4();
		iPacket->Decode1();
		iPacket->Decode1();
		iPacket->Decode1();
	}

	for (int i = 1; i <= 5; ++i)
		mSlotCount->aSlotCount[i] = iPacket->Decode1();

	DecodeInventoryData(iPacket, bForInternal);

	if (flag & 0x100)
	{
		DecodeSkillRecord(iPacket);
	}

	if (flag & 0x8000)
	{
		int nCoolDownSize = iPacket->Decode2();
		for (int i = 0; i < nCoolDownSize; ++i)
		{

		}
	}

	if (flag & 0x200)
	{
		iPacket->Decode1();
		int nStartedQuestSize = iPacket->Decode2();
		for (int i = 0; i < nStartedQuestSize; ++i)
		{
			GW_QuestRecord *pRecord = AllocObj(GW_QuestRecord);
			pRecord->nCharacterID = nCharacterID;
			pRecord->nState = 1;
			pRecord->Decode(iPacket, 1);
			mQuestRecord.insert({ pRecord->nQuestID, pRecord });
		}
		iPacket->Decode2();
	}

	if (flag & 0x4000)
	{
		iPacket->Decode1();
		int nCompletedQuestSize = iPacket->Decode2();
		for (int i = 0; i < nCompletedQuestSize; ++i)
		{
			GW_QuestRecord *pRecord = AllocObj(GW_QuestRecord);
			pRecord->nCharacterID = nCharacterID;
			pRecord->nState = 2;
			pRecord->Decode(iPacket, 2);
			mQuestComplete.insert({ pRecord->nQuestID, pRecord });
		}
	}

	if (flag & 0x400)
	{
		iPacket->Decode2();
	}

	if (flag & 0x800)
	{
		int nCoupleRingSize = iPacket->Decode2();
		int nFriendRingSize = iPacket->Decode2();
		int nMarriageRingSize = iPacket->Decode2();
	}

	if (flag & 0x1000) //ROCK INFO
	{
		for (int i = 0; i < 5; ++i)
			iPacket->Decode4();
		for (int i = 0; i < 10; ++i)
			iPacket->Decode4();
		for (int i = 0; i < 13; ++i)
			iPacket->Decode4();
	}

	if (flag & 0x4000) //QUEST INFO
	{
		int nQuestInfoSize = iPacket->Decode2();
	}

	if (flag & 0x20)
	{
		iPacket->Decode2(); //UNK
	}

	iPacket->Decode1();

	if (flag & 0x40000000)
	{
		iPacket->Decode4(); //UNK
	}

	if (flag & 0x1000)
	{
		iPacket->Decode4(); //UNK
	}

	if (flag & 0x200000)
	{
		//JAGUAR INFO
	}

	//flag & 0x800 && is Zero

	if (flag & 0x4000000)
	{
		iPacket->Decode2(); //UNK
	}

	if (flag & 0x20000000)
	{
		for (int i = 0; i < 15; ++i)
			iPacket->Decode4();
	}

	if (flag & 0x10000000)
	{
		for (int i = 0; i < 5; ++i)
			iPacket->Decode4();
	}

	if (flag & 0x80000000)
	{
		int nAbilityInfoSize = iPacket->Decode2(); //ABILITY INFO
	}

	if (flag & 0x10000)
	{
		iPacket->Decode2(); //UNK
	}

	if (flag & 0x01)
	{
		int nHonorLVL = iPacket->Decode4(); //HONOR LEVEL
		int nHonorEXP = iPacket->Decode4(); //HONOR EXP
		iPacket->Decode4();
		iPacket->Decode4();
		iPacket->Decode4();
	}

	if (flag & 0x2000)
	{
		for (int i = 0; i < 5; ++i)
			iPacket->Decode4();
		for (int i = 0; i < 6; ++i)
			iPacket->Decode4();
		for (int i = 0; i < 4; ++i)
			iPacket->Decode4();
		iPacket->Decode8(); //TIME
		iPacket->Decode1();
		iPacket->Decode1();
	}

	if (flag & 0x4000)
	{
		iPacket->Decode2(); //UNK
	}

	if (flag & 0x8000) //UNK
	{
		iPacket->Decode2();
	}

	if (flag & 0x10000) //UNK
	{
		for (int i = 0; i < 14; ++i) //56
			iPacket->Decode4();
	}

	if (flag & 0x20000) //UNK
	{
		iPacket->Decode2();
	}

	if (flag & 0x2) //UNK
	{
		iPacket->Decode1();
		iPacket->Decode2();
	}

	if (flag & 0x4) //UNK
	{
		iPacket->Decode1();
	}

	//ANGELIC BUSTER
	iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();

	if (flag & 0x200000) //UNK
	{
		iPacket->Decode8();
		iPacket->Decode8();
	}

	if (flag & 0x10)
	{
		iPacket->Decode2();
		iPacket->Decode2();
	}

	iPacket->Decode1();
	if (flag & 0x400)
	{
		for (int i = 0; i < 4; ++i)
			iPacket->Decode4();
	}

	for (int i = 0; i < 7; ++i) //28
		iPacket->Decode4();

	if (flag & 0x8000000)
	{
		iPacket->Decode4();
		iPacket->Decode4();
		iPacket->Decode8();
	}

	iPacket->Decode2();
	iPacket->Decode2();
	iPacket->Decode4();

	if (flag & 0x10000000) //UNK
	{
		iPacket->Decode2();
	}

	if (flag & 0x20000000) //V MATRIX
	{
		int nVMatrixRecord = iPacket->Decode4();
	}

	if (flag & 0x40000) //UNK
	{
		iPacket->Decode1();
		iPacket->Decode2();
		iPacket->Decode2();
	}

	if (flag & 0x80000)
	{
		iPacket->Decode1();
	}

	if (flag & 0x100000)
	{
		iPacket->Decode4();
		iPacket->Decode4();
	}
}

void GA_Character::DecodeStat(InPacket *iPacket)
{
	nCharacterID = iPacket->Decode4();
	nCharacterID = iPacket->Decode4();
	nWorldID = iPacket->Decode4();

	char cStrNameBuff[16]{ 0 };
	iPacket->DecodeBuffer((unsigned char*)cStrNameBuff, 15);
	strName = std::string(cStrNameBuff);

	mStat->nGender = iPacket->Decode1();
	iPacket->Decode1();
	mStat->nSkin = iPacket->Decode1();
	mStat->nFace = iPacket->Decode4();
	mStat->nHair = iPacket->Decode4();
	iPacket->Decode1();
	iPacket->Decode1();
	iPacket->Decode1();
	mLevel->nLevel = iPacket->Decode1(); //LEVEL
	mStat->nJob = iPacket->Decode2();

	mStat->nStr = iPacket->Decode2();
	mStat->nDex = iPacket->Decode2();
	mStat->nInt = iPacket->Decode2();
	mStat->nLuk = iPacket->Decode2();
	mStat->nHP = iPacket->Decode4();
	mStat->nMaxHP = iPacket->Decode4();
	mStat->nMP = iPacket->Decode4();
	mStat->nMaxMP = iPacket->Decode4();

	mStat->nAP = iPacket->Decode2();
	mStat->DecodeExtendSP(iPacket);
	//iPacket->Decode1(); //SP

	mStat->nExp = iPacket->Decode8(); //EXP
	mStat->nFame = iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode8(); //Gach EXP
	iPacket->Decode8(); //
	nFieldID = iPacket->Decode4();
	iPacket->Decode1(); //Inital Spawn Point
	iPacket->Decode2(); //Get Subcategory

	if (WvsGameConstants::IsDemonSlayerJob(mStat->nJob) || WvsGameConstants::IsXenonJob(mStat->nJob))
		mStat->nFaceMark = iPacket->Decode4();
						 //isDemonSlayer || isXenon || isDemonAvenger Decode4

	iPacket->Decode1(); //Fatigue
	iPacket->Decode4();

	for (int i = 0; i < 6; ++i)
		iPacket->Decode4();
	for (int i = 0; i < 6; ++i)
		iPacket->Decode2();
	iPacket->Decode1();
	iPacket->Decode8();

	iPacket->Decode4(); //PVP EXP
	iPacket->Decode1(); //PVP RANK
	iPacket->Decode4(); //BATTLE POINTS
	iPacket->Decode1();
	iPacket->Decode1();
	iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode4();
	iPacket->Decode8();
	iPacket->Decode4();
	iPacket->Decode1();

	for (int i = 0; i < 9; ++i)
	{
		iPacket->Decode4();
		iPacket->Decode1();
		iPacket->Decode4();
	}

	iPacket->Decode8();
	iPacket->Decode8();
	iPacket->Decode8();
	iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode1();

	for (int i = 0; i < 25; ++i)
		iPacket->Decode1();

	iPacket->Decode1();
	iPacket->Decode1();
	iPacket->Decode1();
	iPacket->Decode1();
	iPacket->Decode1();
}

void GA_Character::DecodeInventoryData(InPacket *iPacket, bool bForInternal)
{
	//bForInternal = true 代表Center與Game, Shop之前的傳遞，作用在於標記哪些物品已刪除，便於存檔時直接修改掉CharacterID
	if (bForInternal)
		DecodeInventoryRemovedRecord(iPacket);

	iPacket->Decode8(); // TIME
	iPacket->Decode1();

	unsigned short wPos = 0; //w = WORD

	while ((wPos = iPacket->Decode2()) != 0)
	{
		GW_ItemSlotEquip* eqp = AllocObj(GW_ItemSlotEquip);
		eqp->nPOS = wPos * -1;
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
		eqp->Decode(iPacket, bForInternal);
		mItemSlot[1].insert({ eqp->nPOS , eqp });
	}

	while ((wPos = iPacket->Decode2()) != 0)
	{
		GW_ItemSlotEquip* eqp = AllocObj(GW_ItemSlotEquip);
		eqp->nPOS = (wPos + 100) * -1;
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
		eqp->Decode(iPacket, bForInternal);
		mItemSlot[1].insert({ eqp->nPOS, eqp });
	}

	while ((wPos = iPacket->Decode2()) != 0)
	{
		GW_ItemSlotEquip* eqp = AllocObj(GW_ItemSlotEquip);
		eqp->nPOS = wPos;
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
		eqp->Decode(iPacket, bForInternal);
		mItemSlot[1].insert({ eqp->nPOS, eqp });
	}
	//sub_69B50A
	iPacket->Decode2(); //sub_69B50A 1
	iPacket->Decode2(); //sub_69B50A 2
	iPacket->Decode2(); //sub_69B50A 3
	iPacket->Decode2(); //sub_69B50A 4
	iPacket->Decode2(); //sub_69B50A 5
	iPacket->Decode2(); //sub_69B50A 6
	iPacket->Decode2(); //sub_69B50A 7
	iPacket->Decode2(); //sub_69B50A 8
	iPacket->Decode2(); //sub_69B50A 9
	iPacket->Decode2(); //sub_69B50A 10
	iPacket->Decode2(); //sub_69B50A 11

						 //sub_68CB08
	iPacket->Decode2(); //sub_68CB08 1
	iPacket->Decode2(); //sub_68CB08 2

						 //sub_68CF44
	iPacket->Decode2(); //sub_68CF44 1
	iPacket->Decode2(); //sub_68CF44 2

	unsigned char nPos = 0;
	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBundle* bundle = AllocObj(GW_ItemSlotBundle);
		bundle->nPOS = nPos;
		bundle->nType = GW_ItemSlotBase::GW_ItemSlotType::CONSUME;
		bundle->Decode(iPacket, bForInternal);
		mItemSlot[2].insert({ bundle->nPOS, bundle });
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBundle* bundle = AllocObj(GW_ItemSlotBundle);
		bundle->nPOS = nPos;
		bundle->nType = GW_ItemSlotBase::GW_ItemSlotType::INSTALL;
		bundle->Decode(iPacket, bForInternal);
		mItemSlot[3].insert({ bundle->nPOS, bundle });
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBundle* bundle = AllocObj(GW_ItemSlotBundle);
		bundle->nPOS = nPos;
		bundle->nType = GW_ItemSlotBase::GW_ItemSlotType::ETC;
		bundle->Decode(iPacket, bForInternal);
		mItemSlot[4].insert({ bundle->nPOS, bundle });
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBase* pCash = nullptr;
		int nType = iPacket->Decode1();
		iPacket->Offset(-1);
		if (nType == 3)
			pCash = AllocObj(GW_ItemSlotPet);
		else
			pCash = AllocObj(GW_ItemSlotBundle);

		pCash->nPOS = nPos;
		pCash->nType = GW_ItemSlotBase::GW_ItemSlotType::CASH;
		pCash->Decode(iPacket, bForInternal);
		mItemSlot[5].insert({ pCash->nPOS, pCash });
	}

	iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();

	iPacket->Decode4();
	iPacket->Decode4();

	iPacket->Decode1();
}

void GA_Character::DecodeInventoryRemovedRecord(InPacket * iPacket)
{
	long long int liItemSN_ = -1;
	for (int i = 1; i <= 4; ++i)
		while ((liItemSN_ = iPacket->Decode8()), liItemSN_ != -1)
			mItemRemovedRecord[i].insert(liItemSN_);
}

void GA_Character::DecodeAvatarLook(InPacket * iPacket)
{
	mAvatarData->Decode(iPacket);
}

void GA_Character::DecodeSkillRecord(InPacket * iPacket)
{
	bool flag = iPacket->Decode1() == 1;
	if (flag)
	{
		short nCount = iPacket->Decode2();
		for (int i = 0; i < nCount; ++i)
		{
			GW_SkillRecord* pSkillRecord = AllocObj(GW_SkillRecord);
			pSkillRecord->nMasterLevel = 0;
			pSkillRecord->Decode(iPacket);
			pSkillRecord->nCharacterID = nCharacterID;
			mSkillRecord.insert({ pSkillRecord->nSkillID, pSkillRecord });
		}
		iPacket->Decode2();
		iPacket->Decode4();
	}
}

void GA_Character::EncodeCharacterData(OutPacket *oPacket, bool bForInternal)
{
	long long int flag = 0xFFFFFFFFFFFFFFFF 	;
	oPacket->Encode8(flag);
	oPacket->Encode1(0);
	for (int i = 0; i < 3; ++i)
		oPacket->Encode4(-2);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode1(0);

	if (flag & 1)
	{
		EncodeStat(oPacket);
		oPacket->Encode1(25); //BUDDY LIST CAPACITY

		oPacket->Encode1(0); //BLESS OF FAIRY ORIGIN
		oPacket->Encode1(0); //BLESS OF EMPRESS ORIGIN
		oPacket->Encode1(0); //ULTRA EXPLORER

		oPacket->Encode8(GameDateTime::GetCurrentDate()); //TIME CURRENT TIME
		oPacket->Encode8(GameDateTime::TIME_UNLIMITED); //TIME -2
		oPacket->Encode4(0);
		oPacket->Encode1((char)0xFF);
		oPacket->Encode4(0);
		oPacket->Encode1((char)0xFF);
	}

	if (flag & 2)
	{
		oPacket->Encode8(mMoney->nMoney);
		oPacket->Encode4(nCharacterID);
		oPacket->Encode4(100);
		oPacket->Encode4(20);
	}

	if (flag & 8)
		oPacket->Encode4(0);

	if (flag & 0x80)
	{
		oPacket->Encode4(0);
		oPacket->Encode4(nCharacterID);
		for (int i = 0; i < 3; ++i)
		{
			oPacket->Encode4(0);
			oPacket->Encode4(0);
		}
		oPacket->Encode4(0);
		oPacket->Encode1(0);
		oPacket->Encode1(0);
		oPacket->Encode1(0);
	}

	for (int i = 1; i <= 5; ++i)
		oPacket->Encode1(mSlotCount->aSlotCount[i]);

	EncodeInventoryData(oPacket, bForInternal);

	if (flag & 0x100)
	{
		EncodeSkillRecord(oPacket);
	}

	if (flag & 0x8000)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x200)
	{
		//oPacket->EncodeHexString("01 51 00 CE 74 00 00 00 00 82 69 00 00 01 00 31 84 69 00 00 01 00 31 C5 05 00 00 0A 00 63 6F 75 6E 74 3D 31 30 30 30 78 64 00 00 00 00 38 7C 00 00 00 00 5F 38 00 00 00 00 2C 3B 00 00 03 00 30 30 30 57 71 00 00 00 00 A5 81 00 00 00 00 A6 81 00 00 00 00 DB 81 00 00 00 00 F1 81 00 00 00 00 F3 81 00 00 00 00 84 99 00 00 03 00 30 30 30 85 99 00 00 03 00 30 30 30 86 99 00 00 03 00 30 30 30 87 99 00 00 03 00 30 30 30 88 99 00 00 03 00 30 30 30 89 99 00 00 03 00 30 30 30 8A 99 00 00 03 00 30 30 30 8B 99 00 00 03 00 30 30 30 8C 99 00 00 03 00 30 30 30 8D 99 00 00 03 00 30 30 30 8E 99 00 00 03 00 30 30 30 8F 99 00 00 03 00 30 30 30 90 99 00 00 03 00 30 30 30 91 99 00 00 03 00 30 30 30 92 99 00 00 03 00 30 30 30 93 99 00 00 03 00 30 30 30 94 99 00 00 03 00 30 30 30 95 99 00 00 03 00 30 30 30 96 99 00 00 03 00 30 30 30 97 99 00 00 03 00 30 30 30 98 99 00 00 03 00 30 30 30 99 99 00 00 03 00 30 30 30 9A 99 00 00 03 00 30 30 30 9B 99 00 00 03 00 30 30 30 9C 99 00 00 03 00 30 30 30 9D 99 00 00 03 00 30 30 30 9E 99 00 00 03 00 30 30 30 9F 99 00 00 03 00 30 30 30 A0 99 00 00 03 00 30 30 30 A1 99 00 00 03 00 30 30 30 A2 99 00 00 03 00 30 30 30 A3 99 00 00 03 00 30 30 30 A4 99 00 00 03 00 30 30 30 A5 99 00 00 03 00 30 30 30 A6 99 00 00 03 00 30 30 30 A7 99 00 00 03 00 30 30 30 A8 99 00 00 03 00 30 30 30 A9 99 00 00 03 00 30 30 30 AA 99 00 00 03 00 30 30 30 AB 99 00 00 03 00 30 30 30 AC 99 00 00 03 00 30 30 30 AD 99 00 00 03 00 30 30 30 AE 99 00 00 03 00 30 30 30 AF 99 00 00 03 00 30 30 30 B0 99 00 00 06 00 30 30 30 30 30 30 B1 99 00 00 03 00 30 30 30 B2 99 00 00 03 00 30 30 30 B3 99 00 00 03 00 30 30 30 B4 99 00 00 03 00 30 30 30 B5 99 00 00 03 00 30 30 30 B6 99 00 00 03 00 30 30 30 B7 99 00 00 06 00 30 30 30 30 30 30 B8 99 00 00 06 00 30 30 30 30 30 30 BA 99 00 00 03 00 30 30 30 BB 99 00 00 03 00 30 30 30 BC 99 00 00 06 00 30 30 30 30 30 30 BD 99 00 00 06 00 30 30 30 30 30 30 BE 99 00 00 03 00 30 30 30 BF 99 00 00 03 00 30 30 30 C1 99 00 00 03 00 30 30 30 C2 99 00 00 03 00 30 30 30 C3 99 00 00 06 00 30 30 30 30 30 30 C4 99 00 00 03 00 30 30 30 CC 74 00 00 00 00 CD 74 00 00 00 00 CF 74 00 00 00 00 F0 7D 00 00 00 00 00 00");
		oPacket->Encode1(1);
		oPacket->Encode2((short)mQuestRecord.size());
		for (auto& record : mQuestRecord)
			record.second->Encode(oPacket);
		oPacket->Encode2(0);
	}

	if (flag & 0x4000)
	{
		//oPacket->EncodeHexString("01 07 00 C6 05 00 00 90 5D D3 01 00 00 00 00 C2 05 00 00 F6 5F D3 01 00 00 00 00 C3 05 00 00 F6 5F D3 01 00 00 00 00 C4 05 00 00 F6 5F D3 01 00 00 00 00 BA 05 00 00 90 5D D3 01 00 00 00 00 B9 05 00 00 F6 5F D3 01 00 00 00 00 FA 49 00 00 92 5C D3 01 00 00 00 00");
		oPacket->Encode1(1);
		oPacket->Encode2((short)mQuestComplete.size());
		for (auto& record : mQuestComplete)
			record.second->Encode(oPacket);
	}

	if (flag & 0x400)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x800)
	{
		oPacket->Encode2(0);
		oPacket->Encode2(0);
		oPacket->Encode2(0);
	}

	if (flag & 0x1000) //ROCK INFO
	{
		for (int i = 0; i < 5; ++i)
			oPacket->Encode4(0);
		for (int i = 0; i < 10; ++i)
			oPacket->Encode4(0);
		for (int i = 0; i < 13; ++i)
			oPacket->Encode4(0);
	}

	if (flag & 0x4000) //QUEST INFO
	{
		//oPacket->EncodeHexString("05 00 7A 69 00 00 10 00 65 6E 74 65 72 3D 30 30 30 31 30 30 30 30 30 30 78 69 00 00 1A 00 65 6E 74 65 72 3D 31 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 BE 05 00 00 1B 00 6F 6E 3D 31 3B 72 65 6D 61 69 6E 3D 30 3B 65 78 70 3D 35 30 30 30 30 30 30 30 30 BF 05 00 00 1B 00 6F 6E 3D 31 3B 72 65 6D 61 69 6E 3D 30 3B 65 78 70 3D 35 30 30 30 30 30 30 30 30 C0 05 00 00 1B 00 6F 6E 3D 31 3B 72 65 6D 61 69 6E 3D 30 3B 65 78 70 3D 35 30 30 30 30 30 30 30 30");
		oPacket->Encode2(0);
	}

	if (flag & 0x20)
	{
		oPacket->Encode2(0);
	}

	oPacket->Encode1(1);

	if (flag & 0x40000000)
	{
		oPacket->Encode4(0);
	}

	if (flag & 0x1000)
	{
		oPacket->Encode4(0);
	}

	if (flag & 0x200000)
	{
		//JAGUAR INFO
	}

	//flag & 0x800 && is Zero

	if (flag & 0x4000000)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x20000000)
	{
		for (int i = 0; i < 15; ++i)
			oPacket->Encode4(0);
	}

	if (flag & 0x10000000)
	{
		for (int i = 0; i < 5; ++i)
			oPacket->Encode4(0);
	}

	if (flag & 0x80000000)
	{
		oPacket->Encode2(0); //ABILITY INFO
	}

	if (flag & 0x10000)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x01)
	{
		oPacket->Encode4(0); //HONOR LEVEL
		oPacket->Encode4(0); //HONOR EXP
		oPacket->Encode4(0);
		oPacket->Encode4(0);
		oPacket->Encode4(0);
	}

	if (flag & 0x2000)
	{
		for(int i = 0; i < 5; ++i)
			oPacket->Encode4(0);
		for (int i = 0; i < 6; ++i)
			oPacket->Encode4(0);
		for (int i = 0; i < 4; ++i)
			oPacket->Encode4(0);
		oPacket->Encode8(GameDateTime::TIME_UNLIMITED); //TIME
		oPacket->Encode1(0);
		oPacket->Encode1(1);
	}

	if (flag & 0x4000)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x8000)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x10000)
	{
		for (int i = 0; i < 14; ++i) //56
			oPacket->Encode4(0);
	}

	if (flag & 0x20000)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x2)
	{
		oPacket->Encode1(1);
		oPacket->Encode2(0);
	}

	if (flag & 0x4)
	{
		oPacket->Encode1(0);
	}

	//ANGELIC BUSTER
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	oPacket->Encode1(0);
	oPacket->Encode4(-1);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	if (flag & 0x200000)
	{
		oPacket->Encode8(0);
		oPacket->Encode8(0);
	}

	if (flag & 0x10)
	{
		oPacket->Encode2(0);
		oPacket->Encode2(0);
	}

	oPacket->Encode1(0);
	if (flag & 0x400)
	{
		for (int i = 0; i < 4; ++i)
			oPacket->Encode4(0);
	}

	for (int i = 0; i < 7; ++i) //28
		oPacket->Encode4(0);

	if (flag & 0x8000000)
	{
		oPacket->Encode4(0);
		oPacket->Encode4(0);
		oPacket->Encode8(0);
	}

	oPacket->Encode2(0);
	oPacket->Encode2(0);
	oPacket->Encode4(0);

	if (flag & 0x10000000)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x20000000) //V MATRIX
	{
		oPacket->Encode4(0);
	}

	if (flag & 0x40000)
	{
		oPacket->Encode1(0);
		oPacket->Encode2(0);
		oPacket->Encode2(0);
	}

	if (flag & 0x80000)
	{
		oPacket->Encode1(0);
	}

	if (flag & 0x100000)
	{
		oPacket->Encode4(0);
		oPacket->Encode4(0);
	}
}

void GA_Character::EncodeInventoryData(OutPacket *oPacket, bool bForInternal)
{
	//bForInternal = true 代表Center與Game, Shop之前的傳遞，作用在於標記哪些物品已刪除，便於存檔時直接修改掉CharacterID
	if (bForInternal)
		EncodeInventoryRemovedRecord(oPacket);

	oPacket->Encode8(GameDateTime::TIME_UNLIMITED); // TIME
	oPacket->Encode1(0);

	for (const auto &eqp : mItemSlot[1])
		if (eqp.second->nPOS < 0 && eqp.second->nPOS > -100) 
			eqp.second->Encode(oPacket, bForInternal);

	oPacket->Encode2(0); //EQUIPPED

	for (const auto &eqp : mItemSlot[1])
		if (eqp.second->nPOS <= -100 && eqp.second->nPOS > -1000) 
			eqp.second->Encode(oPacket, bForInternal);
	oPacket->Encode2(0); //EQUIPPED 2

	for (const auto &eqp : mItemSlot[1])
		if (eqp.second->nPOS >= 0) //
			eqp.second->Encode(oPacket, bForInternal);
	oPacket->Encode2(0);
						 //sub_69B50A
	oPacket->Encode2(0); //sub_69B50A 1
	oPacket->Encode2(0); //sub_69B50A 2
	oPacket->Encode2(0); //sub_69B50A 3
	oPacket->Encode2(0); //sub_69B50A 4
	oPacket->Encode2(0); //sub_69B50A 5
	oPacket->Encode2(0); //sub_69B50A 6
	oPacket->Encode2(0); //sub_69B50A 7
	oPacket->Encode2(0); //sub_69B50A 8
	oPacket->Encode2(0); //sub_69B50A 9
	oPacket->Encode2(0); //sub_69B50A 10
	oPacket->Encode2(0); //sub_69B50A 11

						 //sub_68CB08
	oPacket->Encode2(0); //sub_68CB08 1
	oPacket->Encode2(0); //sub_68CB08 2

						 //sub_68CF44
	oPacket->Encode2(0); //sub_68CF44 1
	oPacket->Encode2(0); //sub_68CF44 2

	for (auto &item : mItemSlot[2]) 
		item.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //USE

	for (auto &item : mItemSlot[3])
		item.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //INS

	for (auto &item : mItemSlot[4])
		item.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //ETC

	for (auto &item : mItemSlot[5])
		item.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //CASH

	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	oPacket->Encode4(0);
	oPacket->Encode4(0);

	oPacket->Encode1(0);
}

void GA_Character::EncodeInventoryRemovedRecord(OutPacket * oPacket)
{
	for (int i = 1; i <= 4; ++i)
	{
		for (const auto& liSN : mItemRemovedRecord[i])
			oPacket->Encode8(liSN);
		oPacket->Encode8(-1);
	}
}

GA_Character::ATOMIC_COUNT_TYPE GA_Character::InitCharacterID()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT MAX(CharacterID) From Characters";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0)
		return 0;
	return (ATOMIC_COUNT_TYPE)recordSet["MAX(CharacterID)"];
}

void GA_Character::LoadItemSlot()
{

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT ItemSN FROM ItemSlot_EQP Where CharacterID = " << nCharacterID << " AND POS < " << GW_ItemSlotBase::LOCK_POS;
	queryStatement.execute(); 
	Poco::Data::RecordSet recordSet(queryStatement);

	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext()) 
	{
		GW_ItemSlotEquip *eqp = AllocObj(GW_ItemSlotEquip);
		eqp->Load(recordSet["ItemSN"]);
		mItemSlot[1][eqp->nPOS] = eqp;
	}

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT ItemSN FROM ItemSlot_CON Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext()) {
		GW_ItemSlotBundle *eqp = AllocObj(GW_ItemSlotBundle);
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::CONSUME;
		eqp->Load(recordSet["ItemSN"]);
		mItemSlot[2][eqp->nPOS] = eqp;
	}

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT ItemSN FROM ItemSlot_INS Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext()) 
	{
		GW_ItemSlotBundle *eqp = AllocObj(GW_ItemSlotBundle);
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::INSTALL;
		eqp->Load(recordSet["ItemSN"]);
		mItemSlot[3][eqp->nPOS] = eqp;
	}

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT ItemSN FROM ItemSlot_ETC Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext()) 
	{
		GW_ItemSlotBundle *eqp = AllocObj(GW_ItemSlotBundle);
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::ETC;
		eqp->Load(recordSet["ItemSN"]);
		mItemSlot[4][eqp->nPOS] = eqp;
	}

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT CashItemSN FROM ItemSlot_Cash Where CharacterID = " << nCharacterID << " AND POS < " << GW_ItemSlotBase::LOCK_POS;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext()) 
	{
		GW_ItemSlotBundle *eqp = AllocObj(GW_ItemSlotBundle);
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::CASH;
		eqp->Load(recordSet["CashItemSN"]);
		mItemSlot[5][eqp->nPOS] = eqp;
	}

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT CashItemSN FROM ItemSlot_Pet Where CharacterID = " << nCharacterID << " AND POS < " << GW_ItemSlotBase::LOCK_POS;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext()) 
	{
		GW_ItemSlotPet *pet = AllocObj(GW_ItemSlotPet);
		pet->Load(recordSet["CashItemSN"]);
		mItemSlot[5][pet->nPOS] = pet;
	}
}

void GA_Character::LoadSkillRecord()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM SkillRecord Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext()) 
	{
		GW_SkillRecord* pSkillRecord = AllocObj(GW_SkillRecord);
		pSkillRecord->Load((void*)&recordSet);
		mSkillRecord.insert({ pSkillRecord->nSkillID, pSkillRecord });
	}
}

void GA_Character::LoadQuestRecord()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM QuestRecord Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext()) 
	{
		GW_QuestRecord* pQuestRecord = AllocObj(GW_QuestRecord);
		pQuestRecord->Load((void*)&recordSet);
		if(pQuestRecord->nState == 1)
			mQuestRecord.insert({ pQuestRecord->nQuestID, pQuestRecord });
		else
			mQuestComplete.insert({ pQuestRecord->nQuestID, pQuestRecord });
	}
}
