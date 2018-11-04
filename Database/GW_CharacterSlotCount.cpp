#include "GW_CharacterSlotCount.h"
#include "WvsUnified.h"

void GW_CharacterSlotCount::Load(int nCharacterID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM CharacterSlotCount Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	aSlotCount[1] = (int)recordSet["EquipSlot"];
	aSlotCount[2] = (int)recordSet["ConSlot"];
	aSlotCount[3] = (int)recordSet["InstallSlot"];
	aSlotCount[4] = (int)recordSet["EtcSlot"];
	aSlotCount[5] = (int)recordSet["CashSlot"];
}

void GW_CharacterSlotCount::Save(int nCharacterID, bool bIsNewCharacter)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	if (bIsNewCharacter)
		queryStatement << "INSERT INTO CharacterSlotCount(CharacterID, EquipSlot, ConSlot, InstallSlot, EtcSlot, CashSlot) VALUES("
		<< nCharacterID << ","
		<< aSlotCount[1] << ","
		<< aSlotCount[2] << ","
		<< aSlotCount[3] << ","
		<< aSlotCount[4] << ","
		<< aSlotCount[5] << ")";
	else
		queryStatement << "UPDATE CharacterSlotCount Set " 
		<< "EquipSlot = '" << aSlotCount[1] << "', "
		<< "ConSlot = '" << aSlotCount[2] << "', "
		<< "InstallSlot = '" << aSlotCount[3] << "', "
		<< "EtcSlot = '" << aSlotCount[4] << "', "
		<< "CashSlot = '" << aSlotCount[5] << "' Where CharacterID = " << nCharacterID;
	queryStatement.execute();
}
