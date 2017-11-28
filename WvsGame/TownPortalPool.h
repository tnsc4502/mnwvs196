#pragma once
#include <map>
#include <list>
#include "FieldPoint.h"

class TownPortal;

/*
地圖中管理召喚物 (如召喚門)
*/
class TownPortalPool
{
	std::list<FieldPoint> m_aTownPortal;
public:
	TownPortalPool();
	~TownPortalPool();

	void AddTownPortalPos(FieldPoint pos);
};

