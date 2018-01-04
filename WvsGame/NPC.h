#pragma once
#include "FieldObj.h"

class Npc : public FieldObj
{
public:
	Npc();
	~Npc();

	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void EncodeInitData(OutPacket *oPacket);
};

