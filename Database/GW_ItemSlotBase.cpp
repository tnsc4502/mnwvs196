#include "GW_ItemSlotBase.h"
#include "WvsUnified.h"


GW_ItemSlotBase::GW_ItemSlotBase()
{
}


GW_ItemSlotBase::~GW_ItemSlotBase()
{
}

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
		return 0;
	return (ATOMIC_COUNT_TYPE)recordSet["MAX(ItemSN)"];
}