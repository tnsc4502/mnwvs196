#pragma once

class ClientSocket;
class OutPacket;
class InPacket;
class AsyncScheduler;
struct GA_Character;

class User
{
	int m_nCharacterID, m_nChannelID;
	ClientSocket *m_pSocket;
	GA_Character *m_pCharacterData;
	AsyncScheduler *m_pUpdateTimer;

public:
	User(ClientSocket *pSocket, InPacket *iPacket);
	~User();
	int GetUserID() const;
	void SendPacket(OutPacket *oPacket);
	void OnPacket(InPacket *iPacket);	
	void Update();
	void OnMigrateOutCashShop();
};