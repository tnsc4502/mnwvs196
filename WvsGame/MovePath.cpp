#include "MovePath.h"

#include <..\WvsLib\Net\InPacket.h>
#include <..\WvsLib\Net\OutPacket.h>

MovePath::MovePath()
{
}


MovePath::~MovePath()
{
}

void MovePath::Decode(InPacket * iPacket)
{
	m_tEncodedGatherDuration = iPacket->Decode4();
	m_x = iPacket->Decode2();
	m_y = iPacket->Decode2();
	m_vx = iPacket->Decode2();
	m_vy = iPacket->Decode2();
	int nCount = iPacket->Decode1();
	for (int i = 0; i < nCount; ++i)
	{
		ELEM elem;
		elem.nAttr = iPacket->Decode1();
		switch (elem.nAttr)
		{
			case 0:
			case 8:
			case 15:
			case 17:
			case 19:
			case 67:
			case 68:
			case 69: {
				elem.x = iPacket->Decode2();
				elem.y = iPacket->Decode2();
				elem.vx = iPacket->Decode2();
				elem.vy = iPacket->Decode2();
				elem.fh = iPacket->Decode2();
				
				if (elem.nAttr == 15 || elem.nAttr == 17)
					elem.fhFootStart = iPacket->Decode2();

				elem.offsetX = iPacket->Decode2();
				elem.offsetY = iPacket->Decode2();
				break;
			}
			case 56:
			case 66:
			case 86: {
				elem.x = iPacket->Decode2();
				elem.y = iPacket->Decode2();
				elem.vx = iPacket->Decode2();
				elem.vy = iPacket->Decode2();
				elem.fh = iPacket->Decode2();
				break;
			}
			case 1:
			case 2:
			case 18:
			case 21:
			case 22:
			case 24:
			case 62:
			case 63:
			case 64:
			case 65: {
				elem.vx = iPacket->Decode2();
				elem.vy = iPacket->Decode2();
				if (elem.nAttr == 21 || elem.nAttr == 22)
					elem.fhFootStart = iPacket->Decode2();
				break;
			}
			case 29:
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
			case 49:
			case 50:
			case 51:
			case 55:
			case 57:
				//case 58: 
			case 59:
			case 60:
			case 61:
			case 70:
			case 71:
			case 72:
			case 74:
			case 79:
			case 81:
			case 83:
			case 84: {
				elem.x = m_x;
				elem.y = m_y;
				break;
			}
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 9:
			case 10:
			case 11:
			case 13:
			case 26:
			case 27:
			case 52:
			case 53:
			case 54:
			case 58:
				//case 61:
			case 76:
			case 77:
			case 78:
			case 80:
			case 82: {
				elem.x = iPacket->Decode2();
				elem.y = iPacket->Decode2();
				elem.fh = iPacket->Decode2();
				break;
			}
			case 14:
			case 16: {
				elem.vx = iPacket->Decode2();
				elem.vy = iPacket->Decode2();
				elem.fhFootStart = iPacket->Decode2();
				break;
			}
			case 23: {
				elem.x = iPacket->Decode2();
				elem.y = iPacket->Decode2();
				elem.vx = iPacket->Decode2();
				elem.vy = iPacket->Decode2();
				break;
			}
			case 12: {
				elem.x = m_x;
				elem.y = m_y;
				elem.bStat = iPacket->Decode1();
				break;
			}
		}
		if (elem.nAttr != 12)
		{
			elem.bMoveAction = iPacket->Decode1();
			elem.tElapse = iPacket->Decode2();
			elem.bForcedStop = iPacket->Decode1();
		}
		m_lElem.push_back(std::move(elem));
	}
}

void MovePath::Encode(OutPacket * oPacket)
{
	oPacket->Encode4(m_tEncodedGatherDuration);
	oPacket->Encode2(m_x);
	oPacket->Encode2(m_y);
	oPacket->Encode2(m_vx);
	oPacket->Encode2(m_vy);
	oPacket->Encode1((char)m_lElem.size());
	for (const auto& elem : m_lElem)
	{
		oPacket->Encode1(elem.nAttr);
		switch (elem.nAttr)
		{
			case 0:
			case 8:
			case 15:
			case 17:
			case 19:
			case 67:
			case 68:
			case 69: {
				oPacket->Encode2(elem.x);
				oPacket->Encode2(elem.y);
				oPacket->Encode2(elem.vx);
				oPacket->Encode2(elem.vy);
				oPacket->Encode2(elem.fh);

				if (elem.nAttr == 15 || elem.nAttr == 17)
					oPacket->Encode2(elem.fhFootStart);

				oPacket->Encode2(elem.offsetX);
				oPacket->Encode2(elem.offsetY);
				m_fhLast = elem.fh;
				break;
			}
			case 56:
			case 66:
			case 86: {
				oPacket->Encode2(elem.x);
				oPacket->Encode2(elem.y);
				oPacket->Encode2(elem.vx);
				oPacket->Encode2(elem.vy);
				oPacket->Encode2(elem.fh);
				m_fhLast = elem.fh;
				break;
			}
			case 1:
			case 2:
			case 18:
			case 21:
			case 22:
			case 24:
			case 62:
			case 63:
			case 64:
			case 65: {
				oPacket->Encode2(elem.vx);
				oPacket->Encode2(elem.vy);
				if (elem.nAttr == 21 || elem.nAttr == 22)
					oPacket->Encode2(elem.fhFootStart);
				break;
			}
			case 29:
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
			case 49:
			case 50:
			case 51:
			case 55:
			case 57:
				//case 58: 
			case 59:
			case 60:
			case 61:
			case 70:
			case 71:
			case 72:
			case 74:
			case 79:
			case 81:
			case 83:
			case 84: {
				break;
			}
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 9:
			case 10:
			case 11:
			case 13:
			case 26:
			case 27:
			case 52:
			case 53:
			case 54:
			case 58:
				//case 61:
			case 76:
			case 77:
			case 78:
			case 80:
			case 82: {
				oPacket->Encode2(elem.x);
				oPacket->Encode2(elem.y);
				oPacket->Encode2(elem.fh);
				m_fhLast = elem.fh;
				break;
			}
			case 14:
			case 16: {				
				oPacket->Encode2(elem.vx);
				oPacket->Encode2(elem.vy);
				oPacket->Encode2(elem.fhFootStart);
				break;
			}
			case 23: {
				oPacket->Encode2(elem.x);
				oPacket->Encode2(elem.y);
				oPacket->Encode2(elem.vx);
				oPacket->Encode2(elem.vy);
				break;
			}
			case 12: {
				oPacket->Encode1(elem.bStat);
				break;
			}
		}
		if (elem.nAttr != 12)
		{
			oPacket->Encode1(elem.bMoveAction);
			oPacket->Encode2(elem.tElapse);
			oPacket->Encode1(elem.bForcedStop);
		}
	}
	oPacket->Encode1(0); //Key Pad ?
	oPacket->EncodeBuffer(nullptr, 18); //Dont Know
}
