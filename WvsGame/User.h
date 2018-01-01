#pragma once
#include "FieldObj.h"
#include <mutex>
#include <map>
#include "TemporaryStat.h"

class ClientSocket;
class OutPacket;
class Field;
class Portal;
class InPacket;
struct GA_Character;

class BasicStat;
class SecondaryStat;
struct TemporaryStat;

class User : public FieldObj
{
	friend class USkill;

private:
	std::mutex m_mtxUserlock;
	int nCharacterID;
	ClientSocket *pSocket;
	Field *pField;
	GA_Character *pCharacterData;
	BasicStat* m_pBasicStat;
	SecondaryStat* m_pSecondaryStat;

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

	void OnAvatarModified();
	void EncodeCoupleInfo(OutPacket *oPacket);
	void EncodeFriendshipInfo(OutPacket *oPacket);
	void EncodeMarriageInfo(OutPacket *oPacket);

	void ValidateStat();
	void SendCharacterStat(bool bOnExclRequest, long long int liFlag);
	void SendTemporaryStatReset(long long int uFlag);
	void SendTemporaryStatSet(TemporaryStat::TS_Flag& flag, int tDelay);

	SecondaryStat* GetSecondaryStat();
	BasicStat* GetBasicStat();
};

