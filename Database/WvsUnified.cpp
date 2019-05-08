#include "WvsUnified.h"
#include "Poco\Data\Data.h"
#include "Poco\Data\Statement.h"
#include "DBConstants.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "Poco\Data\MySQL\Connector.h"

typedef WvsUnified::ResultType ResultType;

WvsUnified* stWvsUnifiedDB = new WvsUnified();

WvsUnified::WvsUnified()
	: mDBSessionPool((Poco::Data::MySQL::Connector::registerConnector(), Poco::Data::MySQL::Connector::KEY),
		"host=" + ConfigLoader::Get("GlobalSetting.txt")->StrValue("DB_Host") +
		";user=" + ConfigLoader::Get("GlobalSetting.txt")->StrValue("DB_User") +
		";password=" + ConfigLoader::Get("GlobalSetting.txt")->StrValue("DB_Pass") +
		";db=" + ConfigLoader::Get("GlobalSetting.txt")->StrValue("DB_Name") +
		";character-set=big5;auto-reconnect=true")
{
	if (!mDBSessionPool.get().isConnected())
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

Poco::Data::Session WvsUnified::GetDBSession()
{
	return (mDBSessionPool.get());
	// TODO: insert return statement here
}
