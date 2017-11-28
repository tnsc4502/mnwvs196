#pragma once
#include "FieldObj.h"

class MobTemplate;

class Mob : public FieldObj
{
private:
	const MobTemplate* m_pMobTemplate;

public:
	Mob();
	~Mob();

	void MakeEnterFieldPacket(OutPacket *oPacket);
	void SetMobTemplate(MobTemplate *pTemplate);
	const MobTemplate* GetMobTemplate() const;

	void SetMovePosition(int x, int y, bool bMoveAction, short nSN)
	{
		//v6 = m_stat.nDoom_ == 0;
		SetPosX(x);

	}
};

