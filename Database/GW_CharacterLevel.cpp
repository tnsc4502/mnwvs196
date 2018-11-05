#include "GW_CharacterLevel.h"
#include "WvsUnified.h"

GW_CharacterLevel::GW_CharacterLevel()
{
}

GW_CharacterLevel::~GW_CharacterLevel()
{
}

void GW_CharacterLevel::Load(int nCharacterID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT Level FROM CharacterLevel Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	nLevel = recordSet["Level"];
}

void GW_CharacterLevel::Save(int nCharacterID, bool bNewCharacter)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	if (bNewCharacter)
		queryStatement << "INSERT INTO CharacterLevel(CharacterID, Level) VALUES("
		<< nCharacterID << ","
		<< (short)nLevel << ")";
	else
		queryStatement << "UPDATE CharacterLevel Set Level = '" << (short)nLevel << "' Where CharacterID = " << nCharacterID;
	queryStatement.execute();
}