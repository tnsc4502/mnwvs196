#include "GA_Character.hpp"
#include "WvsUnified.h"
#include "..\Common\Net\OutPacket.h"

#include <chrono>

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
		aCONItem[i].Load(recordSet["SN"], GW_ItemSlotBase::GW_ItemSlotType::CONSUME);

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM ItemSlot_ETC Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	aETCItem.resize(recordSet.rowCount());
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		aETCItem[i].Load(recordSet["SN"], GW_ItemSlotBase::GW_ItemSlotType::ETC);

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM ItemSlot_INS Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	aINSItem.resize(recordSet.rowCount());
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		aINSItem[i].Load(recordSet["SN"], GW_ItemSlotBase::GW_ItemSlotType::INSTALL);
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

void GA_Character::EncodeAvatarLook(OutPacket *oPacket)
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
		eqp.Save(nCharacterID, GW_ItemSlotBase::GW_ItemSlotType::EQUIP);
	for (auto& con : aCONItem)
		con.Save(nCharacterID, GW_ItemSlotBase::GW_ItemSlotType::CONSUME);
	for (auto& etc : aETCItem)
		etc.Save(nCharacterID, GW_ItemSlotBase::GW_ItemSlotType::ETC);
	for (auto& ins : aINSItem)
		ins.Save(nCharacterID, GW_ItemSlotBase::GW_ItemSlotType::INSTALL);
}

void GA_Character::DecodeCharacterData(InPacket *iPacket)
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

	int nEquipSlotCount = iPacket->Decode1(); //EQP SLOT
	int nConsumeSlotCount = iPacket->Decode1(); //CON SLOT
	int nInstallSlotCount = iPacket->Decode1(); //INS SLOT
	int nETCSlotCount = iPacket->Decode1(); //ETC SLOT
	int nCashSlotCount = iPacket->Decode1(); //CASH SLOT

	DecodeInventoryData(iPacket);

	if (flag & 0x100)
	{
		iPacket->Decode1();
		int nSkillSize = iPacket->Decode2();
		for (int i = 0; i < nSkillSize; ++i)
		{

		}
		iPacket->Decode2();
		iPacket->Decode4();
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

		}
		iPacket->Decode2();
	}

	if (flag & 0x4000)
	{
		iPacket->Decode1();
		int nCompletedQuestSize = iPacket->Decode2();
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

	nGender = iPacket->Decode1();
	iPacket->Decode1();
	mAvatarData->nSkin = iPacket->Decode1();
	mAvatarData->nFace = iPacket->Decode4();
	mAvatarData->nHair = iPacket->Decode4();
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
	iPacket->Decode1(); //SP

	mStat->nExp = iPacket->Decode8(); //EXP
	nFame = iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode8(); //Gach EXP
	iPacket->Decode8(); //
	nFieldID = iPacket->Decode4();
	iPacket->Decode1(); //Inital Spawn Point
	iPacket->Decode2(); //Get Subcategory

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

void GA_Character::DecodeInventoryData(InPacket *iPacket)
{
	iPacket->Decode8(); // TIME
	iPacket->Decode1();

	unsigned short wPos = 0; //w = WORD

	while ((wPos = iPacket->Decode2()) != 0)
	{
		GW_ItemSlotEquip eqp;
		eqp.nPOS = wPos;
		eqp.nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
		eqp.Decode(iPacket);
		aEquipItem.push_back(eqp);
	}

	while ((wPos = iPacket->Decode2()) != 0)
	{
		GW_ItemSlotEquip eqp;
		eqp.nPOS = wPos;
		eqp.nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
		eqp.Decode(iPacket);
		aEquipItem.push_back(eqp);
	}


	while ((wPos = iPacket->Decode2()) != 0)
	{
		GW_ItemSlotEquip eqp;
		eqp.nPOS = wPos;
		eqp.nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
		eqp.Decode(iPacket);
		aEquipItem.push_back(eqp);
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
		GW_ItemSlotBundle bundle;
		bundle.nPOS = nPos;
		bundle.nType = GW_ItemSlotBase::GW_ItemSlotType::CONSUME;
		bundle.Decode(iPacket);
		aCONItem.push_back(bundle);
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBundle bundle;
		bundle.nPOS = nPos;
		bundle.nType = GW_ItemSlotBase::GW_ItemSlotType::INSTALL;
		bundle.Decode(iPacket);
		aINSItem.push_back(bundle);
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBundle bundle;
		bundle.nPOS = nPos;
		bundle.nType = GW_ItemSlotBase::GW_ItemSlotType::ETC;
		bundle.Decode(iPacket);
		aETCItem.push_back(bundle);
	}

	iPacket->Decode1(); //CASH

	iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();

	iPacket->Decode4();
	iPacket->Decode4();

	iPacket->Decode1();
}

void GA_Character::EncodeCharacterData(OutPacket *oPacket)
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

		oPacket->EncodeTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())); //TIME CURRENT TIME
		oPacket->EncodeTime(-2); //TIME -2
		oPacket->Encode4(0);
		oPacket->Encode1(0xFF);
		oPacket->Encode4(0);
		oPacket->Encode1(0xFF);
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

	oPacket->Encode1(64); //EQP SLOT
	oPacket->Encode1(64); //CON SLOT
	oPacket->Encode1(64); //INS SLOT
	oPacket->Encode1(64); //ETC SLOT
	oPacket->Encode1(64); //CASH SLOT

	EncodeInventoryData(oPacket);

	if (flag & 0x100)
	{
		oPacket->Encode1(1);
		oPacket->Encode2(0);
		oPacket->Encode2(0);
		oPacket->Encode4(0);
	}

	if (flag & 0x8000)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x200)
	{
		oPacket->Encode1(1);
		oPacket->Encode2(0);
		oPacket->Encode2(0);
	}

	if (flag & 0x4000)
	{
		oPacket->Encode1(1);
		oPacket->Encode2(0);
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
		oPacket->EncodeTime(-2); //TIME
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

void GA_Character::EncodeInventoryData(OutPacket *oPacket)
{
	oPacket->EncodeTime(-2); // TIME
	oPacket->Encode1(0);

	for (const auto &eqp : aEquipItem)
		if (eqp.nPOS < 0 && eqp.nPOS > -100)
			eqp.Encode(oPacket);
	oPacket->Encode2(0); //EQUIPPED

	for (const auto &eqp : aEquipItem)
		if (eqp.nPOS <= -100 && eqp.nPOS > -1000)
			eqp.Encode(oPacket);
	oPacket->Encode2(0); //EQUIPPED 2

	for (const auto &eqp : aEquipItem)
		if (eqp.nPOS >= 0)
			eqp.Encode(oPacket);
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

	for (auto &item : aCONItem)
		item.Encode(oPacket);
	oPacket->Encode1(0); //USE

	for (auto &item : aINSItem)
		item.Encode(oPacket);
	oPacket->Encode1(0); //INS

	for (auto &item : aETCItem)
		item.Encode(oPacket);
	oPacket->Encode1(0); //ETC


	oPacket->Encode1(0); //CASH

	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	oPacket->Encode4(0);
	oPacket->Encode4(0);

	oPacket->Encode1(0);
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