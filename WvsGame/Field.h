#pragma once
#include <map>
#include "User.h"

class LifePool;

class Field
{
	std::map<int, User*> mUser;
	int nFieldID = 0;
	LifePool *pLifePool;

public:
	Field();
	~Field();

	void SetFieldID(int nFieldID)
	{
		this->nFieldID = nFieldID;
	}

	int GetFieldID() const
	{
		return nFieldID;
	}

	void InitLifePool();

	LifePool *GetLifePool()
	{
		return pLifePool;
	}

	void OnEnter(User *pUser);
	void SplitSendPacket(OutPacket *oPacket, User *pExcept);
	void OnPacket(InPacket *iPacket);
};

