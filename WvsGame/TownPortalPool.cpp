#include "TownPortalPool.h"



TownPortalPool::TownPortalPool()
{
}


TownPortalPool::~TownPortalPool()
{
}

void TownPortalPool::AddTownPortalPos(FieldPoint pos)
{
	m_aTownPortal.push_back(pos);
}
