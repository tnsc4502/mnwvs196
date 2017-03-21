#pragma once

#include <string>
#include <vector>
#include "Poco\Data\Session.h"

class WvsUnified
{
protected:
	Poco::Data::Session mDBSession;

public:
	WvsUnified();
	~WvsUnified();

	void InitDB();

	void GetCharacterIdList(int nAccountID, std::vector<int>& result);
};

