#include "GW_ItemSlotBase.h"
#include "WvsUnified.h"

GW_ItemSlotBase::GW_ItemSlotBase()
{
	liItemSN = -1;
}


GW_ItemSlotBase::~GW_ItemSlotBase()
{
}

/*
Load Item SN Atomic Value
*/
GW_ItemSlotBase::ATOMIC_COUNT_TYPE GW_ItemSlotBase::InitItemSN(GW_ItemSlotType type)
{
	std::string strTableName = "";
	if (type == GW_ItemSlotType::EQUIP)
		strTableName = "ItemSlot_EQP";
	else if (type == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (type == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (type == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT MAX(ItemSN) From " << strTableName;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0 || recordSet["MAX(ItemSN)"].isEmpty())
		return 2;
	auto result = (ATOMIC_COUNT_TYPE)recordSet["MAX(ItemSN)"];
	return result > 1 ? result : 2;
}

/*
Encode Item Type, Where Equip = 1, Stackable = 2, Pet = 3
*/
void GW_ItemSlotBase::Encode(OutPacket *oPacket, bool bForInternal) const
{
	oPacket->Encode1(nType == EQUIP ? 1 : (bIsPet ? 3 : 2)); //Pet = 3
}

/*
Encode Basic Item Information
*/
void GW_ItemSlotBase::RawEncode(OutPacket *oPacket) const
{
	GW_ItemSlotBase::Encode(oPacket, false);
	oPacket->Encode4(nItemID);
	bool isCashItem = liCashItemSN != -1; //liCashItemSN.QuadPart
	oPacket->Encode1(isCashItem); //
	if (isCashItem)
		oPacket->Encode8(liCashItemSN);	
	oPacket->Encode8(liExpireDate);
	oPacket->Encode4(-1); //Extended Slot ?
}

/*
Encode Inventory Item Position
*/
void GW_ItemSlotBase::EncodeInventoryPosition(OutPacket *oPacket) const
{
	auto encodePos = nPOS;
	if (encodePos <= -1)
	{
		encodePos *= -1;
		if (encodePos > 100 && encodePos < 1000)
			encodePos -= 100;
	}
	if (bIsInBag)
		oPacket->Encode4(encodePos % 100 - 1);
	else if (nType == GW_ItemSlotType::EQUIP)
		oPacket->Encode2(encodePos);
	else
	{
		oPacket->Encode1((char)encodePos);
	}
}

/*
Trading System Item Position Encoding
*/
void GW_ItemSlotBase::EncodeTradingPosition(OutPacket *oPacket) const
{
	auto encodePos = nPOS;
	if (encodePos <= -1)
	{
		encodePos *= -1;
		if (encodePos > 100 && encodePos < 1000)
			encodePos -= 100;
	}
	oPacket->Encode1((char)encodePos);
}

bool GW_ItemSlotBase::IsTreatSingly() const
{
	int nItemHeader = nItemID / 1000000;
	return (nItemHeader != 2 && nItemHeader != 3 && nItemHeader != 4
		|| nItemID / 10000 == 207
		/*|| liExpireDate != 0*/);
}

void GW_ItemSlotBase::Decode(InPacket *iPacket, bool bForInternal)
{
	bIsPet = (iPacket->Decode1() == 3);
}

void GW_ItemSlotBase::RawDecode(InPacket *iPacket)
{
	GW_ItemSlotBase::Decode(iPacket, false);
	nItemID = iPacket->Decode4();
	bool isCashItem = iPacket->Decode1() == 1;
	if (isCashItem)
		liCashItemSN = iPacket->Decode8();
	liExpireDate = iPacket->Decode8();
	iPacket->Decode4(); //Extended Slot?
}