#include "WvsPhysicalSpace2D.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "StaticFoothold.h"
#include "FootholdTree.h"

void WvsPhysicalSpace2D::Load(void * pPropFoothold, void * pLadderRope, void * pInfo)
{
	auto& refPropFoothold = *((WZ::Node*)pPropFoothold);
	auto& refLadderRope = *((WZ::Node*)pLadderRope);
	auto& refInfo = *((WZ::Node*)pInfo);

	StaticFoothold *pFoothold = nullptr;
	long long int lPage = 0, lZMass = 0;
	for (auto& page : refPropFoothold)
	{
		lPage = atoll(page.Name().c_str());
		for (auto& zMass : page)
		{
			lZMass = atoll(zMass.Name().c_str());
			for (auto& foothold : zMass)
			{
				pFoothold = new StaticFoothold;
				pFoothold->m_nSN = atoi(foothold.Name().c_str());
				pFoothold->m_lPage = lPage;
				pFoothold->m_lZMass = lZMass;
				pFoothold->m_ptPos1.x = (int)foothold["x1"];
				pFoothold->m_ptPos1.y = (int)foothold["y1"];
				pFoothold->m_ptPos2.x = (int)foothold["x2"];
				pFoothold->m_ptPos2.y = (int)foothold["y2"];
				pFoothold->m_nSNPrev = (int)foothold["prev"];
				pFoothold->m_nSNNext = (int)foothold["next"];
				pFoothold->ValidateVectorInfo();
				m_pTree->InsertData(
					pFoothold->m_ptPos1,
					pFoothold->m_ptPos2,
					pFoothold
				);
			}
		}
	}
}

WvsPhysicalSpace2D::WvsPhysicalSpace2D()
{
	m_pTree = new FootholdTree;
}


WvsPhysicalSpace2D::~WvsPhysicalSpace2D()
{
}
