#pragma once
#include "WvsUnified.h"

class SocketBase;

class CharacterDBAccessor
{
private:
	static WvsUnified mDBUnified;

public:
	CharacterDBAccessor();
	~CharacterDBAccessor();

	CharacterDBAccessor* GetInstance()
	{
		static CharacterDBAccessor* sPtrAccessor = new CharacterDBAccessor();
		return sPtrAccessor;
	}

	void PostLoadCharacterListRequest(SocketBase *pSrv, int uLocalSocketSN, int nAccountID, int nWorldID);
};

