#include "GW_ItemSlotPet.h"
#include "WvsUnified.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "Poco\Data\MySQL\MySQLException.h"

GW_ItemSlotPet::GW_ItemSlotPet()
{
	strPetName.resize(32);
	bIsPet = true;
	bIsCash = true;
	nInstanceType = GW_ItemSlotInstanceType::GW_ItemSlotPet_Type;
}


GW_ItemSlotPet::~GW_ItemSlotPet()
{
}

void GW_ItemSlotPet::Load(ATOMIC_COUNT_TYPE SN)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM ItemSlot_Pet Where CashItemSN = " << SN;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	nCharacterID = recordSet["CharacterID"];
	liCashItemSN = recordSet["CashItemSN"];
	nItemID = recordSet["ItemID"];
	liExpireDate = recordSet["ExpireDate"];
	nAttribute = recordSet["Attribute"];
	nPetAttribute = (short)recordSet["PetAttribute"];
	nPOS = recordSet["POS"];
	nLevel = (unsigned char)(unsigned short)recordSet["Level"];
	nRepleteness = (unsigned char)(unsigned short)recordSet["Repleteness"];
	nTameness = (short)recordSet["Tameness"];
	usPetSkill = (unsigned short)recordSet["PetSkill"];
	strPetName = recordSet["PetName"].toString();
	nActiveState = (unsigned char)(unsigned short)recordSet["ActiveState"];
	nAutoBuffSkill = recordSet["AutoBuffSkill"];
	nPetHue = recordSet["PetHue"];
	nGiantRate = recordSet["GiantRate"];

	nType = GW_ItemSlotType::CASH;
}

void GW_ItemSlotPet::Save(int nCharacterID)
{
	if (nType != GW_ItemSlotType::CASH)
		throw std::runtime_error("Invalid Equip Type.");

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	try 
	{
		if (liItemSN < -1) //DROPPED or DELETED
		{
			liItemSN *= -1;
			queryStatement << "UPDATE ItemSlot_Pet "
				<< " Set CharacterID = -1 Where CharacterID = " << nCharacterID
				<< " and  CashItemSN = " << liCashItemSN;
			queryStatement.execute();
			return;
		}
		if (liCashItemSN == -1)
		{
			liCashItemSN = IncItemSN(GW_ItemSlotBase::CASH);
			queryStatement << "INSERT INTO ItemSlot_Pet (CashItemSN, ItemID, CharacterID, ExpireDate, Attribute, PetAttribute, POS, Level, Repleteness, Tameness, PetSkill, PetName, RemainLife, ActiveState, AutoBuffSkill, PetHue, GiantRate) VALUES("
				<< liCashItemSN << ", "
				<< nItemID << ", "
				<< nCharacterID << ", "
				<< liExpireDate << ", "
				<< nAttribute << ", "
				<< nPetAttribute << ", "
				<< nPOS << ", "
				<< (int)nLevel << ", "
				<< (int)nRepleteness << ", "
				<< nTameness << ", "
				<< usPetSkill << ", "
				<< "'" << strPetName << "', "
				<< nRemainLife << ", "
				<< (int)nActiveState << ", "
				<< nAutoBuffSkill << ", "
				<< nPetHue << ", "
				<< nGiantRate << ")";
		}
		else
		{
			queryStatement << "UPDATE ItemSlot_Pet Set "
				<< "ItemID = " << nItemID << ", "
				<< "CharacterID = " << nCharacterID << ", "
				<< "ExpireDate = '" << liExpireDate << "', "
				<< "Attribute = " << nAttribute << ", "
				<< "POS = " << nPOS << ", "
				<< "Attribute = " << nAttribute << ", "
				<< "PetAttribute = " << nPetAttribute << ", "
				<< "Level = " << (int)nLevel << ", "
				<< "Repleteness = " << (int)nRepleteness << ", "
				<< "Tameness = " << (int)nTameness << ", "
				<< "PetSkill = " << (int)usPetSkill << ", "
				<< "PetName = '" << strPetName << "', "
				<< "RemainLife = " << (int)nRemainLife << ", "
				<< "ActiveState = " << (int)nActiveState << ", "
				<< "AutoBuffSkill = " << (int)nAutoBuffSkill << ", "
				<< "PetHue = " << (int)nPetHue << ", "
				<< "GiantRate = " << (int)nGiantRate << " WHERE CashItemSN = " << liCashItemSN;
		}
		//std::cout << "Qeury : " << queryStatement.toString() << std::endl;
		queryStatement.execute();
	}
	catch (Poco::Data::MySQL::StatementException &e) 
	{
		printf("SQL Exception : %s\n", e.what());
	}
}

void GW_ItemSlotPet::Encode(OutPacket * oPacket, bool bForInternal) const
{
	EncodeInventoryPosition(oPacket);
	RawEncode(oPacket);
}

void GW_ItemSlotPet::RawEncode(OutPacket * oPacket) const
{
	GW_ItemSlotBase::RawEncode(oPacket);	
	oPacket->EncodeBuffer(
		(unsigned char*)strPetName.data(),
		(int)strPetName.size(),
		13 - (int)strPetName.size());

	oPacket->Encode1(nLevel);
	oPacket->Encode2(nTameness);
	oPacket->Encode1(nRepleteness);
	oPacket->Encode8(liExpireDate);
	oPacket->Encode2(nPetAttribute);
	oPacket->Encode2(usPetSkill);
	oPacket->Encode4(nRemainLife);
	oPacket->Encode2(nAttribute);
	oPacket->Encode1(nActiveState);
	oPacket->Encode4(nAutoBuffSkill);
	oPacket->Encode4(nPetHue);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(-1);
	oPacket->Encode2(nGiantRate);
}

void GW_ItemSlotPet::Decode(InPacket * iPacket, bool bForInternal)
{
	RawDecode(iPacket);
}

void GW_ItemSlotPet::RawDecode(InPacket * iPacket)
{
	GW_ItemSlotBase::RawDecode(iPacket);
	char cStrNameBuff[16]{ 0 };
	iPacket->DecodeBuffer((unsigned char*)cStrNameBuff, 13);
	strPetName = cStrNameBuff;

	nLevel = iPacket->Decode1();
	nTameness = iPacket->Decode2();
	nRepleteness = iPacket->Decode1();
	liExpireDate = iPacket->Decode8();
	nPetAttribute = iPacket->Decode2();
	usPetSkill = iPacket->Decode2();
	nRemainLife = iPacket->Decode4();
	nAttribute = iPacket->Decode2();
	nActiveState = iPacket->Decode1();
	nAutoBuffSkill = iPacket->Decode4();
	nPetHue = iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();
	nGiantRate = iPacket->Decode2();
}

void GW_ItemSlotPet::Release()
{
	FreeObj(this);
}

GW_ItemSlotBase * GW_ItemSlotPet::MakeClone()
{
	GW_ItemSlotBase *pPet = AllocObj(GW_ItemSlotPet);
	*pPet = *this;
	pPet->liCashItemSN = -1;
	return pPet;
}
