#pragma once
#include "FieldObj.h"

class User;
class InPacket;

class Npc : public FieldObj
{
public:
	Npc();
	~Npc();

	void OnUpdateLimitedInfo(User* pUser, InPacket *iPacket);
	void SendChangeControllerPacket(User* pUser);
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void EncodeInitData(OutPacket *oPacket);
};

