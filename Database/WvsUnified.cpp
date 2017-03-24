#include "WvsUnified.h"
#include "Poco\Data\Data.h"
#include "Poco\Data\Statement.h"
#include "DBConstants.h"
#include "Poco\Data\MySQL\Connector.h"

typedef WvsUnified::ResultType ResultType;

WvsUnified* stWvsUnifiedDB = new WvsUnified();

WvsUnified::WvsUnified()
	: mDBSession((Poco::Data::MySQL::Connector::registerConnector(), Poco::Data::MySQL::Connector::KEY),
	  "host=127.0.0.1;user=" + DBConstants::strDBUser + 
	  ";password=" + DBConstants::strDBPasswd + 
	  ";db=" + DBConstants::strDBName + 
	  ";auto-reconnect=true")
{
	if (!mDBSession.isConnected())
	{
		printf("WvsUnified Init Failed.\n");
		throw std::runtime_error("WvsUnified Init Failed.");
	}
}


WvsUnified::~WvsUnified()
{
}

void WvsUnified::InitDB()
{

}

ResultType WvsUnified::GetCharacterIdList(int nAccountID, int nWorldID)
{
	Poco::Data::Statement queryStatement(mDBSession);
	queryStatement << "SELECT CharacterID FROM Characters Where AccountID = " << nAccountID << " AND WorldID = " << nWorldID;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i)
	{
		std::cout << "Name = " << recordSet["CharacterID"].toString() << std::endl;
	}
	return recordSet;
}

ResultType WvsUnified::LoadAvatar(int nCharacterID)
{
	Poco::Data::Statement queryStatement(mDBSession);
	queryStatement << "Select c.* FROM CharacterAvatar as c Where c.CharacterID = " << nCharacterID;
	/*queryStatement << "INNER JOIN CharacterLevel as cl ON cl.CharacterID = c.CharacterID ";
	queryStatement << "LEFT OUTER JOIN CharacterStat as cs ON cs.CharacterID = c.CharacterID ";
	queryStatement << "LEFT OUTER JOIN CharacterAvatar as ca ON ca.CharacterID = c.CharacterID ";
	queryStatement << "LEFT OUTER JOIN ItemSlot_EQP as iq ON iq.CharacterID = c.CharacterID ";*/
	//queryStatement << "Where c.CharacterID = " << nCharacterID;
	std::cout << queryStatement.toString() << std::endl;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	std::cout << "Result = " << recordSet.rowCount() << std::endl;
	return recordSet;
}