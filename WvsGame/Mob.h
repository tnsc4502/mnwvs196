#pragma once
#include "FieldObj.h"

class Mob : public FieldObj
{
public:
	Mob();
	~Mob();

	void MakeEnterFieldPacket(OutPacket *oPacket);
};

