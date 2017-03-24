#pragma once

#include <string>
#include <vector>
#include "Poco\Data\Session.h"
#include "Poco\Data\RecordSet.h"

#define GET_DB_SESSION WvsUnified::GetInstance()->GetDBSession()

class WvsUnified
{
private:
	Poco::Data::Session mDBSession;

public:
	typedef Poco::Data::RecordSet ResultType;
	WvsUnified();
	~WvsUnified();

	void InitDB();

	static WvsUnified* GetInstance()
	{
		static WvsUnified* pInstance = new WvsUnified();
		return pInstance;
	}

	Poco::Data::Session& GetDBSession()
	{
		return mDBSession;
	}

	ResultType GetCharacterIdList(int nAccountID, int nWorldID);
	ResultType LoadAvatar(int nCharacterID);
};