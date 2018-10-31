#pragma once
#include "FieldPoint.h"

class StaticFoothold
{
	friend class WvsPhysicalSpace2D;

	FieldPoint m_ptPos1, m_ptPos2;
	double m_dLen = 0, m_dVx = 0, m_dVy = 0;
	int m_nSN = 0, m_nSNPrev = 0, m_nSNNext = 0;
	long long int m_lZMass, m_lPage;

public:
	StaticFoothold();
	~StaticFoothold();

	
	void ValidateVectorInfo();
};

