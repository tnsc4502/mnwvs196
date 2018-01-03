#include "GW_ItemSlotBundle.h"
#include "WvsUnified.h"

GW_ItemSlotBundle::GW_ItemSlotBundle()
{
}


GW_ItemSlotBundle::~GW_ItemSlotBundle()
{
}

void GW_ItemSlotBundle::Load(ATOMIC_COUNT_TYPE SN, GW_ItemSlotType type)
{
	std::string strTableName = "";
	if (type == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (type == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (type == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else
		throw std::runtime_error("Invalid Item Slot Type.");
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM " << strTableName << " Where SN = " << SN;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	nCharacterID = recordSet["CharacterID"];
	liItemSN = recordSet["ItemSN"];
	nItemID = recordSet["ItemID"];
	liExpireDate = recordSet["ExpireDate"];
	nAttribute = recordSet["Attribute"];
	nNumber = recordSet["Number"];
	nPOS = recordSet["POS"];
	nType = type;
}

void GW_ItemSlotBundle::Save(int nCharacterID, GW_ItemSlotType type)
{
	std::string strTableName = "";
	if (type == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (type == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (type == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else
		throw std::runtime_error("Invalid Item Slot Type.");
	Poco::Data::Statement queryStatement(GET_DB_SESSION);

	//Note That When Status = DROPPED, It Means That Item Have Been Cousuming Out Or Dropped.
	if (nStatus == GW_ItemSlotStatus::DROPPED)
	{
		queryStatement << "DELETE FROM " << strTableName << " Where ItemSN = " << liItemSN;
		queryStatement.execute();
		return;
	}
	if (liItemSN < -1 /*nStatus == GW_ItemSlotStatus::DROPPED*/) //DROPPED or DELETED
	{
		liItemSN *= -1;
		queryStatement << "UPDATE " << strTableName << " Set CharacterID = -1 Where CharacterID = " << nCharacterID << " and ItemSN = " << liItemSN;
		queryStatement.execute();
		return;
	}
	if (liItemSN == -1)
	{
		liItemSN = IncItemSN(type);
		queryStatement << "INSERT INTO " << strTableName << " (ItemSN, ItemID, CharacterID, ExpireDate, Attribute, POS, Number) VALUES("
			<< liItemSN << ", "
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
			<< "Number = '" << nNumber << "' WHERE ItemSN = " << liItemSN;
	}
	queryStatement.execute();
}

void GW_ItemSlotBundle::Encode(OutPacket *oPacket) const
{
	EncodeInventoryPosition(oPacket);
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
	printf("[GW_ItemSlotBundle::RawEncode] Number = %d Attribute = %d\n", nNumber, nAttribute);
	//          Throwing Start            Bullet
	if ((nItemID / 10000 == 207) || (nItemID / 10000 == 233) || (nItemID / 10000 == 287))
		oPacket->Encode8(liItemSN);
	oPacket->Encode8(liItemSN);
	for (int i = 0; i < 2; ++i)
		oPacket->Encode4(0);
	oPacket->Encode1(0);
}

void GW_ItemSlotBundle::Decode(InPacket *iPacket)
{
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
	liItemSN = iPacket->Decode8();
	iPacket->DecodeBuffer(nullptr, 9);
}

GW_ItemSlotBase * GW_ItemSlotBundle::MakeClone()
{
	GW_ItemSlotBundle* ret = new GW_ItemSlotBundle();
	*ret = *this;
	ret->liItemSN = -1;
	/*OutPacket cloneOut;
	Encode(&cloneOut);
	InPacket cloneIn(cloneOut.GetPacket(), cloneOut.GetPacketSize());
	ret->Decode(&cloneIn);*/
	return ret;
}