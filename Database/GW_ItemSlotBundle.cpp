#include "GW_ItemSlotBundle.h"
#include "WvsUnified.h"

GW_ItemSlotBundle::GW_ItemSlotBundle()
{
}


GW_ItemSlotBundle::~GW_ItemSlotBundle()
{
}

void GW_ItemSlotBundle::Load(int SN, const char* strTableName)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM " << strTableName << " Where SN = " << SN;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	lItemSN = recordSet["ItemSN"];
	nItemID = recordSet["ItemID"];
	lExpireDate = recordSet["ExpireDate"];
	nAttribute = recordSet["Attriubte"];
	nNumber = recordSet["Number"];
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
	if (nStatus == GW_ItemSlotStatus::DROPPED)
	{
		queryStatement << "DELETE FROM " << strTableName << " Where ItemSN = " << lItemSN;
		queryStatement.execute();
		return;
	}
	if (lItemSN == -1)
	{
		lItemSN = IncItemSN(type);
		queryStatement << "INSERT INTO " << strTableName << " (ItemSN, ItemID, CharacterID, ExpireDate, Attribute, POS, Number) VALUES("
			<< lItemSN << ", "
			<< nItemID << ", "
			<< nCharacterID << ", "
			<< lExpireDate << ", "
			<< nAttribute << ", "
			<< nPOS << ", "
			<< nNumber << ")";
	}
	else
	{
		queryStatement << "UPDATE " << strTableName << " Set "
			<< "ItemID = '" << nItemID << "', "
			<< "CharacterID = '" << nCharacterID << "', "
			<< "ExpireDate = '" << lExpireDate << "', "
			<< "Attribute = '" << nAttribute << "', "
			<< "POS ='" << nPOS << "', "
			<< "Number = '" << nNumber << "' WHERE ItemSN = " << lItemSN;
	}
	queryStatement.execute();
}