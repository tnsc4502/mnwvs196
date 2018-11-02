#pragma once
#include <map>
#include <vector>
#include "FieldPoint.h"
#include "FieldRect.h"

class Field;
class StaticFoothold;
class FootholdTree;

class WvsPhysicalSpace2D
{
	std::map<int, StaticFoothold*> m_mFoothold;
	std::vector<StaticFoothold*> m_lFoothold, m_lFootholdFixedMob;
	std::map<long long, std::pair<int, int>> m_mMassRange;
	std::vector<int> m_aIndexZMass;
	std::vector<std::vector<int>> m_aaMassFootholdList;
	FieldRect m_rcMBR;
	FootholdTree* m_pTree;

public:
	const FieldRect& GetRect() const;
	bool IsPointInMBR(int x, int y, bool bAsClient);
	std::vector<FieldPoint> GetFootholdRandom(int nCount, FieldRect& rcRange);
	std::vector<long long int> GetFootholdRange(int x, int y1, int y2);
	StaticFoothold* GetFootholdClosest(Field *pField, int x, int y, int *pcx, int *pcy, int ptHitx);
	StaticFoothold* GetFootholdUnderneath(int x, int y, int *pcy);
	void Load(void *pPropFoothold, void *pLadderRope, void *pInfo);
	WvsPhysicalSpace2D();
	~WvsPhysicalSpace2D();

public:
	template<class T> static  char __SETS__(T x)
	{
		if (sizeof(T) == 1)
			return char(x) < 0;
		if (sizeof(T) == 2)
			return short(x) < 0;
		if (sizeof(T) == 4)
			return int(x) < 0;
		return long long int(x) < 0;
	}

	template<class T, class U> static int __OFSUB__(T x, U y)
	{
		if (sizeof(T) < sizeof(U))
		{
			U x2 = x;
			char sx = __SETS__(x2);
			return (sx ^ __SETS__(y)) & (sx ^ __SETS__(x2 - y));
		}
		else
		{
			T y2 = y;
			char sx = __SETS__(x);
			return (sx ^ __SETS__(y2)) & (sx ^ __SETS__(x - y2));
		}
	}
};

