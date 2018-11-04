#include "GW_Account.h"
#include "WvsUnified.h"

void GW_Account::Load(int nAccount_)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM Account Where AccountID = " << nAccount_;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	this->nAccountID = recordSet["AccountID"];
	nNexonCash = recordSet["NexonCash"];
	nMaplePoint = recordSet["MaplePoint"];
}

int GW_Account::QueryCash(int nChargeType)
{
	std::string strColumnName = 
		(nChargeType == 1 ? "NexonCash" : "MaplePoint");

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT " + strColumnName + " FROM Account Where AccountID = " << nAccountID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0 || recordSet[strColumnName].isEmpty())
		return 0;
	return atoi(recordSet[strColumnName].toString().c_str());
}

void GW_Account::UpdateCash(int nChargeType, int nCharge)
{
	std::string strColumnName = 
		(nChargeType == 1 ? "NexonCash" : "MaplePoint");

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement <<
		"UPDATE Account Set " + strColumnName + " = " + strColumnName + " + "
		<< nCharge << " WHERE AccountID = " << nAccountID;

	queryStatement.execute();
}

GW_Account::GW_Account()
{
}


GW_Account::~GW_Account()
{
}
