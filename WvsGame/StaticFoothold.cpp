#include "StaticFoothold.h"
#include <cmath>

StaticFoothold::StaticFoothold()
{
}


StaticFoothold::~StaticFoothold()
{
}

void StaticFoothold::ValidateVectorInfo()
{
	double dXOffset = m_ptPos2.x - m_ptPos1.x;
	double dYOffset = m_ptPos2.y - m_ptPos1.y;
	double dRange = sqrt(dXOffset * dXOffset + dYOffset + dYOffset);
	m_dLen = dRange;
	m_dVx = dXOffset / dRange;
	m_dVy = dYOffset / dRange;
}

int StaticFoothold::GetSN()
{
	return m_nSN;
}
