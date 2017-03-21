#include "WvsUnified.h"
#include "Poco\Data\Data.h"
#include "DBConstants.h"

WvsUnified::WvsUnified()
	: mDBSession(DBConstants::strDBType, 
	  "user=" + DBConstants::strDBUser + 
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