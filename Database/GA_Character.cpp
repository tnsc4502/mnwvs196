#include "GA_Character.hpp"
#include "WvsUnified.h"
#include "..\Common\Net\OutPacket.h"

GA_Character::GA_Character()
	: mAvatarData(new GW_Avatar()),
	  mStat(new GW_CharacterStat()),
	  mLevel(new GW_CharacterLevel()),
	  mMoney(new GW_CharacterMoney())
{
}

void GA_Character::Load(int nCharacterID)
{
	LoadAvatar(nCharacterID);
	mMoney->Load(nCharacterID);

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM ItemSlot_EQP Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	aEquipItem.resize(recordSet.rowCount());
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		aEquipItem[i].Load(recordSet["SN"]);

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM ItemSlot_CON Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	aCONItem.resize(recordSet.rowCount());
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		aCONItem[i].Load(recordSet["SN"], "ItemSlot_CON");

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM ItemSlot_ETC Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	aETCItem.resize(recordSet.rowCount());
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		aETCItem[i].Load(recordSet["SN"], "ItemSlot_ETC");

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM ItemSlot_INS Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	aINSItem.resize(recordSet.rowCount());
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		aINSItem[i].Load(recordSet["SN"], "ItemSlot_INS");
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
	nFame = recordSet["Fame"];
	nGuildID = recordSet["GuildID"];
	nPartyID = recordSet["PartyID"];
	nFieldID = recordSet["FieldID"];
	nGender = recordSet["Gender"];
}

void GA_Character::Encode(OutPacket *oPacket)
{
	EncodeStat(oPacket);
	EncodeAvatar(oPacket);
}

void GA_Character::EncodeStat(OutPacket *oPacket)
{
	oPacket->Encode4(nCharacterID);
	oPacket->Encode4(nCharacterID);
	oPacket->Encode4(nWorldID);
	oPacket->EncodeBuffer((unsigned char*)strName.c_str(), 15);
	oPacket->Encode1(nGender);
	oPacket->Encode1(0);
	oPacket->Encode1(mAvatarData->nSkin);
	oPacket->Encode4(mAvatarData->nFace);
	oPacket->Encode4(mAvatarData->nHair);
	oPacket->Encode1(0xFF);
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
	oPacket->Encode1(0); //SP

	oPacket->Encode8(mStat->nExp); //EXP
	oPacket->Encode4(nFame);
	oPacket->Encode4(0);
	oPacket->Encode8(0); //Gach EXP
	oPacket->Encode8(-1); //
	oPacket->Encode4(nFieldID);
	oPacket->Encode1(0); //Inital Spawn Point
	oPacket->Encode2(0); //Get Subcategory

	//isDemonSlayer || isXenon || isDemonAvenger Encode4

	oPacket->Encode1(0); //Fatigue
	oPacket->Encode4(0);

	for (int i = 0; i < 6; ++i)
		oPacket->Encode4(0);
	for (int i = 0; i < 6; ++i)
		oPacket->Encode2(0);
	oPacket->Encode1(0);
	oPacket->Encode8(-1);

	oPacket->Encode4(0); //PVP EXP
	oPacket->Encode1(0); //PVP RANK
	oPacket->Encode4(0); //BATTLE POINTS
	oPacket->Encode1(0x05);
	oPacket->Encode1(0x06);
	oPacket->Encode4(0);

	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode8(-4);
	oPacket->Encode4(0);
	oPacket->Encode1(0);

	for (int i = 0; i < 9; ++i)
	{
		oPacket->Encode4(0);
		oPacket->Encode1(0);
		oPacket->Encode4(0);
	}

	oPacket->Encode8(0);
	oPacket->Encode8(0);
	oPacket->Encode8(0);
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

void GA_Character::EncodeAvatar(OutPacket *oPacket)
{
	oPacket->Encode1(nGender);
	oPacket->Encode1(mAvatarData->nSkin);
	oPacket->Encode4(mAvatarData->nFace);
	oPacket->Encode4(mStat->nJob);
	oPacket->Encode1(0); //Mega?
	oPacket->Encode4(mAvatarData->nHair);

	for (const auto& eqp : mAvatarData->aHairEquip)
	{
		oPacket->Encode1(eqp.nPOS * -1);
		oPacket->Encode4(eqp.nItemID);
	}
	oPacket->Encode1(0xFF);
	for (const auto& eqp : mAvatarData->aUnseenEquip)
	{
		oPacket->Encode1((eqp.nPOS * -1) - 100);
		oPacket->Encode4(eqp.nItemID);
	}
	oPacket->Encode1(0xFF);
	oPacket->Encode1(0xFF); //totem

	int cWeaponIdx = -1, weaponIdx = -1, nShieldIdx = -1;
	for (int i = 0; i < mAvatarData->aHairEquip.size(); ++i)
		if (mAvatarData->aHairEquip[i].nPOS == -111)
			cWeaponIdx = i;
		else if (mAvatarData->aHairEquip[i].nPOS == -11)
			weaponIdx = i;
		else if (mAvatarData->aHairEquip[i].nPOS == -10)
			nShieldIdx = i;

	oPacket->Encode4(cWeaponIdx == -1 ? 0 : mAvatarData->aHairEquip[cWeaponIdx].nItemID);
	oPacket->Encode4(weaponIdx == -1 ? 0 : mAvatarData->aHairEquip[weaponIdx].nItemID);
	oPacket->Encode4(nShieldIdx == -1 ? 0 : mAvatarData->aHairEquip[nShieldIdx].nItemID);

	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	//isDemonSlayer || isXenon || isDemonAvenger oPacket->Encode4(0);
	//isZero oPacket->Encode1(0)

	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
}

void GA_Character::Save(bool isNewCharacter)
{
	if (isNewCharacter)
	{
		nCharacterID = IncCharacterID();
		Poco::Data::Statement newRecordStatement(GET_DB_SESSION);
		newRecordStatement << "INSERT INTO Characters(CharacterID, AccountID) VALUES(" << nCharacterID << ", " << nAccountID << ")";
		newRecordStatement.execute();
		newRecordStatement.reset(GET_DB_SESSION);
		//newRecordStatement << "SELECT CharacterID FROM Characters"
	}
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "UPDATE Characters Set "
		<< "WorldID = '" << nWorldID << "', "
		<< "Gender = '" << nGender << "', "
		<< "CharacterName = '" << strName << "', "
		<< "Fame = '" << nFame << "', "
		<< "GuildID = '" << nGuildID << "', "
		<< "PartyID = '" << nPartyID << "', "
		<< "FieldID = '" << nFieldID << "' WHERE CharacterID = " << nCharacterID;
	queryStatement.execute();
	mAvatarData->Save(nCharacterID, isNewCharacter);
	mMoney->Save(nCharacterID, isNewCharacter);
	mLevel->Save(nCharacterID, isNewCharacter);
	mStat->Save(nCharacterID, isNewCharacter);

	for (auto& eqp : aEquipItem)
		eqp.Save(nCharacterID);
	for (auto& con : aCONItem)
		con.Save(nCharacterID, GW_ItemSlotBase::GW_ItemSlotType::CONSUME);
	for (auto& etc : aETCItem)
		etc.Save(nCharacterID, GW_ItemSlotBase::GW_ItemSlotType::ETC);
	for (auto& ins : aINSItem)
		ins.Save(nCharacterID, GW_ItemSlotBase::GW_ItemSlotType::INSTALL);
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