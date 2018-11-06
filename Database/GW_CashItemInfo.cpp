#include "GW_CashItemInfo.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "WvsUnified.h"
#include "Poco\Data\MySQL\MySQLException.h"

GW_CashItemInfo::GW_CashItemInfo()
{
}


GW_CashItemInfo::~GW_CashItemInfo()
{
}

void GW_CashItemInfo::Encode(OutPacket * oPacket)
{
	oPacket->Encode8(liSN);
	oPacket->Encode4(nAccountID);
	oPacket->Encode4(nCharacterID);
	oPacket->Encode4(nItemID);
	oPacket->Encode4(nCommodityID);
	oPacket->Encode2(nNumber);
	oPacket->EncodeBuffer(
		(unsigned char*)sBuyCharacterID.data(), 
		(int)sBuyCharacterID.size(),
		15 - (int)sBuyCharacterID.size()
	);
	oPacket->Encode8(liDateExpire);
	oPacket->Encode4(nPaybackRate);
	long long int* pD = (long long int*)(&dDiscountRate);
	oPacket->Encode8(*pD);
	oPacket->Encode4(nOrderNo);
	oPacket->Encode4(nProductNo);
	oPacket->Encode1(bRefundable);
	oPacket->Encode1(nSourceFlag);
	oPacket->Encode1(nStoreBank);
	cashItemOption.Encode(oPacket);
}

void GW_CashItemInfo::Decode(InPacket * iPacket)
{
	liSN = iPacket->Decode8();
	nAccountID = iPacket->Decode4();
	nCharacterID = iPacket->Decode4();
	nItemID = iPacket->Decode4();
	nCommodityID = iPacket->Decode4();
	nNumber = iPacket->Decode2();
	unsigned char buffer[16] = { 0 };
	iPacket->DecodeBuffer((unsigned char*)&buffer, 15);
	liDateExpire = iPacket->Decode8();
	nPaybackRate = iPacket->Decode4();
	long long int liD = iPacket->Decode8();
	dDiscountRate = *((double*)(&liD));
	nOrderNo = iPacket->Decode4();
	nProductNo = iPacket->Decode4();
	bRefundable = iPacket->Decode1() == 1;
	nSourceFlag = iPacket->Decode1();
	nStoreBank = iPacket->Decode1();
	cashItemOption.Decode(iPacket);
}

void GW_CashItemInfo::Load(long long int liCashItemSN)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM CashItemInfo INNER JOIN ItemLocker ON CashItemInfo.AccountID = ItemLocker.AccountID AND CashItemInfo.CashItemSN = ItemLocker.CashItemSN WHERE ItemLocker.CashItemSN = " << liCashItemSN;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	nAccountID = recordSet["AccountID"];
	nCharacterID = recordSet["CharacterID"];
	nItemID = recordSet["ItemID"];
	nCommodityID = recordSet["CommodityID"];
	nNumber = recordSet["Number"];
	sBuyCharacterID = recordSet["BuyCharacterID"].toString();
	liDateExpire = recordSet["DateExpire"];
	dDiscountRate = (double)recordSet["DiscountRate"];
	nOrderNo = recordSet["OrderNo"];
	nProductNo = recordSet["ProductNo"];
	bRefundable = ((int)recordSet["Refundable"]) == 1;
	nSourceFlag = recordSet["SourceFlag"];
	nStoreBank = recordSet["StoreBank"];
	cashItemOption.liCashItemSN = liCashItemSN;
	cashItemOption.ftExpireDate = recordSet["OptExpireDate"];
	cashItemOption.nGrade = recordSet["OptGrade"];
	for (int i = 1; i <= GW_CashItemOption::OPTION_SIZE; ++i)
		cashItemOption.aOption[i - 1] = (int)recordSet["Opt" + std::to_string(i)];
	nGWItemSlotInstanceType = recordSet["Type"];
	bLocked = ((int)recordSet["Locked"] == 1);
	liSN = cashItemOption.liCashItemSN;
}

void GW_CashItemInfo::Save(bool bNewInstance)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	if (bNewInstance)
	{
		queryStatement << "INSERT INTO CashItemInfo (CashItemSN, AccountID, CharacterID, ItemID, CommodityID, Number, BuyCharacterID, DateExpire, PaybackRate, DiscountRate, OrderNo, ProductNo, Refundable, SourceFlag, StoreBank, OptExpireDate, OptGrade";
		for (int i = 1; i <= GW_CashItemOption::OPTION_SIZE; ++i)
			queryStatement << ", Opt" + std::to_string(i);
		queryStatement << ") VALUES(";

		queryStatement << cashItemOption.liCashItemSN << ", "
			<< nAccountID << ", "
			<< nCharacterID << ", "
			<< nItemID << ", "
			<< nCommodityID << ", "
			<< nNumber << ", "
			<< "'" << sBuyCharacterID << "', "
			<< liDateExpire << ", "
			<< nPaybackRate << ", "
			<< dDiscountRate << ", "
			<< nOrderNo << ", "
			<< nProductNo << ", "
			<< ((int)bRefundable) << ", "
			<< nSourceFlag << ", "
			<< nStoreBank << ", "
			<< cashItemOption.ftExpireDate << ", "
			<< cashItemOption.nGrade << "";
		for (int i = 1; i <= GW_CashItemOption::OPTION_SIZE; ++i)
			queryStatement << ", " 
			<< cashItemOption.aOption[i - 1];

		queryStatement << ")";
		//std::cout << "Query Statement : " << queryStatement.toString() << std::endl;
		queryStatement.execute();
		queryStatement.reset(GET_DB_SESSION);

		queryStatement << "INSERT INTO ItemLocker (CashItemSN, AccountID, Type, Locked) VALUES(";
		queryStatement << cashItemOption.liCashItemSN << ", "
			<< nAccountID << ", "
			<< nGWItemSlotInstanceType << ", "
			<< ((int)bLocked) << ")";

		queryStatement.execute();
		queryStatement.reset(GET_DB_SESSION);
		queryStatement << "SELECT SN FROM CashItemInfo WHERE CashItemSN = " << cashItemOption.liCashItemSN;
		queryStatement.execute();
		Poco::Data::RecordSet recordSet(queryStatement);
		liSN = cashItemOption.liCashItemSN;
	}
	else
	{
		queryStatement << "UPDATE CashItemInfo Set "
			<< "CashItemSN = " << cashItemOption.liCashItemSN << ", "
			<< "AccountID = " << nAccountID << ", "
			<< "CharacterID = " << nCharacterID << ", "
			<< "ItemID = " << nItemID << ", "
			<< "CommodityID = " << nCommodityID << ", "
			<< "Number = " << nNumber << ", "
			<< "BuyCharacterID = '" << sBuyCharacterID << "', "
			<< "DateExpire = " << liDateExpire << ", "
			<< "PaybackRate = " << nPaybackRate << ", "
			<< "DiscountRate = " << dDiscountRate << ", "
			<< "OrderNo = " << nOrderNo << ", "
			<< "ProductNo = " << nProductNo << ", "
			<< "Refundable = " << ((int)bRefundable) << ", "
			<< "SourceFlag = " << nSourceFlag << ", "
			<< "StoreBank = " << nStoreBank << ", "
			<< "OptExpireDate = " << cashItemOption.ftExpireDate << ", "
			<< "OptGrade = " << cashItemOption.nGrade << ", ";

		for (int i = 1; i <= GW_CashItemOption::OPTION_SIZE; ++i)
			queryStatement <<
			"Opt" + std::to_string(i) + " = "
			<< cashItemOption.aOption[i - 1]
			<< (i == GW_CashItemOption::OPTION_SIZE ? " " : ", ");

		queryStatement << " WHERE CashItemSN = " << cashItemOption.liCashItemSN;

		queryStatement.execute();
		queryStatement.reset(GET_DB_SESSION);
		queryStatement << "UPDATE ItemLocker Set "
			<< "CashItemSN = " << cashItemOption.liCashItemSN << ", "
			<< "AccountID = " << nAccountID << ", "
			<< "Type = " << nGWItemSlotInstanceType << ", "
			<< "Locked = " << ((int)bLocked) << " WHERE CashItemSN = " << cashItemOption.liCashItemSN;
		queryStatement.execute();
	}
}

std::vector<GW_CashItemInfo> GW_CashItemInfo::LoadAll(int nAccountID, bool bLockedOnly)
{
	std::vector<GW_CashItemInfo> aRet;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT CashItemSN FROM ItemLocker where AccountID = " << nAccountID;
	if (bLockedOnly)
		queryStatement << " AND Locked = 1";

	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	aRet.resize(recordSet.rowCount());
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext()) 
		aRet[i].Load((long long int)recordSet["CashItemSN"]);
	return aRet;
}

void GW_CashItemInfo::GW_CashItemOption::Encode(OutPacket * oPacket)
{
	oPacket->Encode8(liCashItemSN);
	oPacket->Encode8(ftExpireDate);
	oPacket->Encode4(nGrade);
	for (int i = 0; i < OPTION_SIZE; ++i)
		oPacket->Encode4(aOption[i]);
}

void GW_CashItemInfo::GW_CashItemOption::Decode(InPacket * iPacket)
{
	liCashItemSN = iPacket->Decode8();
	ftExpireDate = iPacket->Decode8();
	nGrade = iPacket->Decode4();
	for (int i = 0; i < OPTION_SIZE; ++i)
		aOption[i] = iPacket->Decode4();
}
