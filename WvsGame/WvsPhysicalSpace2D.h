#pragma once
#include <map>
#include <vector>

class StaticFoothold;
class FootholdTree;

class WvsPhysicalSpace2D
{
public:
	std::map<int, StaticFoothold*> m_mFoothold;
	std::vector<StaticFoothold*> m_lFoothold, m_lFootholdFixedMob;
	std::vector<double> m_aMassRange;
	std::vector<int> m_aIndexZMass;
	std::vector<std::vector<int>> m_aaMassFootholdList;

	FootholdTree* m_pTree;

	void Load(void *pPropFoothold, void *pLadderRope, void *pInfo);
	WvsPhysicalSpace2D();
	~WvsPhysicalSpace2D();
};

