#pragma once
#include "FieldObj.h"

class ClientSocket;
class OutPacket;
class Field;
class InPacket;
class GA_Character;

class User : public FieldObj
{
	int nCharacterID;
	ClientSocket *pSocket;
	Field *pField;
	GA_Character *pCharacterData;
public:
	User() {}
	User(ClientSocket *pSocket, InPacket *iPacket);
	~User();

	int GetUserID() const;

	void MakeEnterFieldPacket(OutPacket *oPacket);
	void SendPacket(OutPacket *oPacket);
	void OnPacket(InPacket *iPacket);

	void OnChat(InPacket *iPacket);
};

