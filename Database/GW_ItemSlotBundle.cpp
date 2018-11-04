#include "GW_ItemSlotBundle.h"
#include "WvsUnified.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "Poco\Data\MySQL\MySQLException.h"

GW_ItemSlotBundle::GW_ItemSlotBundle()
{
	nInstanceType = GW_ItemSlotInstanceType::GW_ItemSlotBundle_Type;
}


GW_ItemSlotBundle::~GW_ItemSlotBundle()
{
}

void GW_ItemSlotBundle::Load(ATOMIC_COUNT_TYPE SN)
{
	std::string strTableName = "",
				sSNColumnName = (nType == GW_ItemSlotType::CASH ? "CashItemSN" : "ItemSN");

	if (nType == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (nType == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (nType == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (nType == GW_ItemSlotType::CASH)
		strTableName = "ItemSlot_CASH";
	else
		throw std::runtime_error("Invalid Item Slot Type.");
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM " << strTableName << " Where " + sSNColumnName + " = " << SN;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	nCharacterID = recordSet["CharacterID"];

	if (nType == GW_ItemSlotType::CASH)
		liCashItemSN = recordSet[sSNColumnName];
	else
		liItemSN = recordSet[sSNColumnName];

	nItemID = recordSet["ItemID"];
	liExpireDate = recordSet["ExpireDate"];
	nAttribute = recordSet["Attribute"];
	nNumber = recordSet["Number"];
	nPOS = recordSet["POS"];

	if (nType == GW_ItemSlotType::CASH)
		bIsCash = true;
}

void GW_ItemSlotBundle::Save(int nCharacterID)
{

	std::string strTableName = "",
		sSNColumnName = (nType == GW_ItemSlotType::CASH ? "CashItemSN" : "ItemSN");
	if (nType == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (nType == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (nType == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (nType == GW_ItemSlotType::CASH)
		strTableName = "ItemSlot_CASH";
	else
		throw std::runtime_error("Invalid Item Slot Type.");
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	try 
	{
		//Note That When Status = DROPPED, It Means That Item Have Been Cousuming Out Or Dropped.
		/*if (nStatus == GW_ItemSlotStatus::DROPPED)
		{
			queryStatement << "DELETE FROM " << strTableName << " Where ItemSN = " << liItemSN;
			queryStatement.execute();
			return;
		}*/
		if (liItemSN < -1 /*nStatus == GW_ItemSlotStatus::DROPPED*/) //DROPPED or DELETED
		{
			liItemSN *= -1;
			queryStatement << "UPDATE " << strTableName
				<< " Set CharacterID = -1 Where CharacterID = " << nCharacterID
				<< " and " + sSNColumnName + " = " << (nType == GW_ItemSlotType::CASH ? liCashItemSN : liItemSN);
			queryStatement.execute();
			return;
		}
		if ((nType == GW_ItemSlotType::CASH ? liCashItemSN : liItemSN) == -1)
		{
			liItemSN = IncItemSN(nType);
			if (nType == GW_ItemSlotType::CASH && liCashItemSN == -1)
				liCashItemSN = liItemSN;
			queryStatement << "INSERT INTO " << strTableName << " (" + sSNColumnName + ", ItemID, CharacterID, ExpireDate, Attribute, POS, Number) VALUES("
				<< (nType == GW_ItemSlotType::CASH ? liCashItemSN : liItemSN) << ", "
				<< nItemID << ", "
				<< nCharacterID << ", "
				<< liExpireDate << ", "
				<< nAttribute << ", "
				<< nPOS << ", "
				<< nNumber << ")";
		}
		else
		{
			queryStatement << "UPDATE " << strTableName << " Set "
				<< "ItemID = '" << nItemID << "', "
				<< "CharacterID = '" << nCharacterID << "', "
				<< "ExpireDate = '" << liExpireDate << "', "
				<< "Attribute = '" << nAttribute << "', "
				<< "POS ='" << nPOS << "', "
				<< "Number = '" << nNumber
				<< "' WHERE " + sSNColumnName + " = " << (nType == GW_ItemSlotType::CASH ? liCashItemSN : liItemSN);
		}
		queryStatement.execute();
	}
	catch (Poco::Data::MySQL::StatementException &) 
	{
		printf("SQL Exception : %s\n", queryStatement.toString().c_str());
	}
}

void GW_ItemSlotBundle::Encode(OutPacket *oPacket, bool bForInternal) const
{
	EncodeInventoryPosition(oPacket);
	if (bForInternal)
		oPacket->Encode8(liItemSN);
	RawEncode(oPacket);
}

/*
Encode Bundle Item Info.
*/
void GW_ItemSlotBundle::RawEncode(OutPacket *oPacket) const
{
	GW_ItemSlotBase::RawEncode(oPacket);
	oPacket->Encode2(nNumber);
	oPacket->EncodeStr("");
	oPacket->Encode2(nAttribute);
	//printf("[GW_ItemSlotBundle::RawEncode] Number = %d Attribute = %d\n", nNumber, nAttribute);
	//          Throwing Start            Bullet
	if ((nItemID / 10000 == 207) || (nItemID / 10000 == 233) || (nItemID / 10000 == 287))
		oPacket->Encode8(liItemSN);
	oPacket->Encode8(0);
	for (int i = 0; i < 2; ++i)
		oPacket->Encode4(0);
	oPacket->Encode1(0);
}

void GW_ItemSlotBundle::Decode(InPacket *iPacket, bool bForInternal)
{
	if (bForInternal)
		liItemSN = iPacket->Decode8();
	RawDecode(iPacket);
}

void GW_ItemSlotBundle::RawDecode(InPacket *iPacket)
{
	GW_ItemSlotBase::RawDecode(iPacket);
	nNumber = iPacket->Decode2();
	std::string strTitle = iPacket->DecodeStr();
	nAttribute = iPacket->Decode2();
	if ((nItemID / 10000 == 207) || (nItemID / 10000 == 233) || (nItemID / 10000 == 287))
		liItemSN = iPacket->Decode8();
	iPacket->Decode8();
	iPacket->DecodeBuffer(nullptr, 9);
}

void GW_ItemSlotBundle::Release()
{
	FreeObj(this);
}

GW_ItemSlotBase * GW_ItemSlotBundle::MakeClone()
{
	GW_ItemSlotBundle* ret = AllocObj(GW_ItemSlotBundle);
	*ret = *this;
	ret->liItemSN = -1;
	ret->liCashItemSN = -1;
	/*OutPacket cloneOut;
	Encode(&cloneOut);
	InPacket cloneIn(cloneOut.GetPacket(), cloneOut.GetPacketSize());
	ret->Decode(&cloneIn);*/
	return ret;
}