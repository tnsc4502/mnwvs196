#include "GW_ItemSlotBase.h"
#include "WvsUnified.h"
#include "GW_ItemSlotBundle.h"
#include "GW_ItemSlotEquip.h"
#include "GW_ItemSlotPet.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

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
	else if (type == GW_ItemSlotType::CASH) 
		strTableName = "ItemSlot_CASH";

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	ATOMIC_COUNT_TYPE result = 0;

	if (type == GW_ItemSlotType::CASH)
	{
		queryStatement << "SELECT MAX(CashItemSN) FROM ItemSlot_Cash UNION ALL SELECT MAX(CashItemSN) FROM ItemSlot_Pet UNION ALL SELECT MAX(CashItemSN) FROM ItemSlot_Eqp";
		queryStatement.execute();
		Poco::Data::RecordSet recordSet(queryStatement);
		for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
			if(!recordSet["MAX(CashItemSN)"].isEmpty())
				result = std::max(result, (ATOMIC_COUNT_TYPE)recordSet["MAX(CashItemSN)"]);
	}
	else
	{
		queryStatement << "SELECT MAX(ItemSN) From " << strTableName;
		queryStatement.execute();
		Poco::Data::RecordSet recordSet(queryStatement);
		if (recordSet.rowCount() == 0 || recordSet["MAX(ItemSN)"].isEmpty())
			return 2;
		result = (ATOMIC_COUNT_TYPE)recordSet["MAX(ItemSN)"];
	}
	return result > 1 ? result : 2;
}

GW_ItemSlotBase::ATOMIC_COUNT_TYPE GW_ItemSlotBase::IncItemSN(GW_ItemSlotType type)
{
	static std::atomic<ATOMIC_COUNT_TYPE> atEqpAtomicCounter = InitItemSN(GW_ItemSlotType::EQUIP);
	static std::atomic<ATOMIC_COUNT_TYPE> atConAtomicCounter = InitItemSN(GW_ItemSlotType::CONSUME);
	static std::atomic<ATOMIC_COUNT_TYPE> atInsAtomicCounter = InitItemSN(GW_ItemSlotType::INSTALL);
	static std::atomic<ATOMIC_COUNT_TYPE> atEtcAtomicCounter = InitItemSN(GW_ItemSlotType::ETC);
	static std::atomic<ATOMIC_COUNT_TYPE> atCashAtomicCounter = InitItemSN(GW_ItemSlotType::CASH);
	if (type == GW_ItemSlotType::EQUIP)
		return ++atEqpAtomicCounter;
	else if (type == GW_ItemSlotType::CONSUME)
		return ++atConAtomicCounter;
	else if (type == GW_ItemSlotType::INSTALL)
		return ++atInsAtomicCounter;
	else if (type == GW_ItemSlotType::ETC)
		return ++atEtcAtomicCounter;
	else if (type == GW_ItemSlotType::CASH)
		return ++atCashAtomicCounter;
}

/*
Encode Item Type, Where Equip = 1, Stackable = 2, Pet = 3
*/
void GW_ItemSlotBase::Encode(OutPacket *oPacket, bool bForInternal) const
{
	oPacket->Encode1(nInstanceType); //Pet = 3
}

/*
Encode Basic Item Information
*/
void GW_ItemSlotBase::RawEncode(OutPacket *oPacket) const
{
	GW_ItemSlotBase::Encode(oPacket, false);
	oPacket->Encode4(nItemID);
	//bool bIsCashItem = liCashItemSN != -1; //liCashItemSN.QuadPart
	oPacket->Encode1(bIsCash); //
	if (bIsCash)
		oPacket->Encode8(liCashItemSN);	
	oPacket->Encode8(liExpireDate);
	oPacket->Encode4(-1); //Extended Slot ?
}

/*
Encode Inventory Item Position
*/
void GW_ItemSlotBase::EncodeInventoryPosition(OutPacket *oPacket) const
{
	auto nEncodePos = nPOS;
	if (nEncodePos <= -1)
	{
		nEncodePos *= -1;
		if (nEncodePos > 100 && nEncodePos < 1000)
			nEncodePos -= 100;
	}
	if (bIsInBag)
		oPacket->Encode4(nEncodePos % 100 - 1);
	else if (nType == GW_ItemSlotType::EQUIP)
		oPacket->Encode2(nEncodePos);
	else
	{
		oPacket->Encode1((char)nEncodePos);
	}
}

/*
Trading System Item Position Encoding
*/
void GW_ItemSlotBase::EncodeTradingPosition(OutPacket *oPacket) const
{
	auto nEncodePos = nPOS;
	if (nEncodePos <= -1)
	{
		nEncodePos *= -1;
		if (nEncodePos > 100 && nEncodePos < 1000)
			nEncodePos -= 100;
	}
	oPacket->Encode1((char)nEncodePos);
}

void GW_ItemSlotBase::DecodeInventoryPosition(InPacket * iPacket) 
{
	if (nType == GW_ItemSlotBase::GW_ItemSlotType::EQUIP)
		nPOS = iPacket->Decode2();
	else
		nPOS = iPacket->Decode1();
}

void GW_ItemSlotBase::Decode(InPacket *iPacket, bool bForInternal)
{
	bIsPet = (iPacket->Decode1() == GW_ItemSlotInstanceType::GW_ItemSlotPet_Type);
}

void GW_ItemSlotBase::RawDecode(InPacket *iPacket)
{
	GW_ItemSlotBase::Decode(iPacket, false);
	nItemID = iPacket->Decode4();
	bIsCash = iPacket->Decode1() == 1;
	if (bIsCash)
		liCashItemSN = iPacket->Decode8();
	liExpireDate = iPacket->Decode8();
	iPacket->Decode4(); //Extended Slot?
}

GW_ItemSlotBase* GW_ItemSlotBase::CreateItem(int nInstanceType)
{
	GW_ItemSlotBase *pRet = nullptr;
	if (nInstanceType == GW_ItemSlotEquip_Type)
		pRet = AllocObj(GW_ItemSlotEquip);
	else if (nInstanceType == GW_ItemSlotBundle_Type)
		pRet = AllocObj(GW_ItemSlotBundle);
	else if (nInstanceType == GW_ItemSlotPet_Type)
		pRet = AllocObj(GW_ItemSlotPet);
	return pRet;
}