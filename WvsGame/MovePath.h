#pragma once
#include <list>
class InPacket;
class OutPacket;

struct MovePath
{
	struct ELEM
	{
		short x = 0, y = 0, vx = 0, vy = 0, offsetX = 0, offsetY = 0, fh = 0, fhFootStart = 0;
		char bMoveAction = 0, bForcedStop = 0, bStat = 0;
		int tElapse = 0, nAttr = 0;
	};
	short m_x, m_y, m_vx, m_vy;
	int m_fhLast, m_tEncodedGatherDuration;

	std::list<ELEM> m_lElem;

	MovePath();
	~MovePath();

	void Decode(InPacket* iPacket);
	void Encode(OutPacket* oPacket);
	
};

