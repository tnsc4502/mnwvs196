#include "GW_CharacterMoney.h"
#include "WvsUnified.h"

GW_CharacterMoney::GW_CharacterMoney()
{
}

GW_CharacterMoney::~GW_CharacterMoney()
{
}

void GW_CharacterMoney::Load(int nCharacterID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT Money FROM CharacterMoney Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	nMoney = recordSet["Money"];
}

void GW_CharacterMoney::Save(int nCharacterID, bool bIsNewCharacter)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	if (bIsNewCharacter)
		queryStatement << "INSERT INTO CharacterMoney(CharacterID, Money) VALUES(" 
		<< nCharacterID << ","
		<< nMoney << ")";
	else
		queryStatement << "UPDATE CharacterMoney Set Money = '" << nMoney << "' Where CharacterID = " << nCharacterID;
	queryStatement.execute();
}