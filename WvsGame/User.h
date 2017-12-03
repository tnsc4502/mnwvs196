#pragma once
#include "FieldObj.h"
#include <mutex>
#include <map>

class ClientSocket;
class OutPacket;
class Field;
class Portal;
class InPacket;
class GA_Character;

class User : public FieldObj
{
	std::mutex m_mtxUserlock;
	int nCharacterID;
	ClientSocket *pSocket;
	Field *pField;
	GA_Character *pCharacterData;

public:
	User() {}
	User(ClientSocket *pSocket, InPacket *iPacket);
	~User();

	int GetUserID() const;

	GA_Character* GetCharacterData();
	Field* GetField();
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void SendPacket(OutPacket *oPacket);
	void OnPacket(InPacket *iPacket);
	void LeaveField();

	void OnTransferFieldRequest(InPacket* iPacket);
	void OnChat(InPacket *iPacket);
	void PostTransferField(int dwFieldID, Portal* pPortal, int bForce);
	void SetMovePosition(int x, int y, char bMoveAction, short nFSN);
};

