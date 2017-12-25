#include "GW_CharacterList.hpp"
#include "WvsUnified.h"

void GW_CharacterList::Load(int nAccountID, int nWorldID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT CharacterID FROM Characters Where AccountID = " << nAccountID << " AND WorldID = " << nWorldID;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	nCount = (int)recordSet.rowCount();
	aCharacterList.resize(nCount);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		aCharacterList[i] = (int)(recordSet["CharacterID"]);
		std::cout << "ID = " << (int)recordSet["CharacterID"] << std::endl;
	}
}