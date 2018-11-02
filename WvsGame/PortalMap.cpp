#include "PortalMap.h"
#include "Portal.h"
#include "TownPortalPool.h"
#include "Field.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include <algorithm>

PortalMap::PortalMap()
{
}


PortalMap::~PortalMap()
{
	for (auto& p : m_apPortal)
		FreeObj( p );
}

/*
載入該地圖中的所有portals
*/
void PortalMap::RestorePortal(Field * pField, void* pImg)
{
	WZ::Node & pPropPortal = *((WZ::Node*)pImg);
	for (auto& portalNode : pPropPortal)
	{
		Portal* newPortal = AllocObjCtor(Portal)(
			atoi(portalNode.Name().c_str()),
			(int)portalNode["pt"],
			(int)portalNode["tm"],
			(int)portalNode["x"],
			(int)portalNode["y"],
			(std::string)portalNode["pn"],
			(std::string)portalNode["tn"],
			""
		);
		int nType = newPortal->GetPortalType();
		if (nType >= 7 && nType <= 9)
			newPortal->SetPortalScriptName(portalNode["portal"]);
		else if (nType == 6)
		{
			FieldPoint pt;
			pt.x = newPortal->GetX();
			pt.y = newPortal->GetY();
			pField->GetTownPortalPool()->AddTownPortalPos(pt);
		}

		if (nType)
			newPortal->SetY(newPortal->GetY() - 40);

		m_apPortal.push_back(newPortal);
		int nIdx = (int)m_apPortal.size() - 1;

		if (!nType)
			m_aStartPoint.push_back(nIdx);
	}
}

void PortalMap::EnablePortal(const std::string & sName, bool bEnable)
{
	for (auto pPortal : m_apPortal)
	{
		if ((pPortal->GetPortalType() == 4 || pPortal->GetPortalType() == 5)
			&& pPortal->GetPortalName() == sName)
			pPortal->SetEnable(bEnable);
	}
}

/*
找到最近的起始點portal
*/
Portal * PortalMap::FindCloseStartPoint(int x, int y)
{
	double dDis = 0, calcDis;
	int nIdx = -1;
	for (auto index : m_aStartPoint)
	{
		auto pPortal = m_apPortal[(int)index];
		calcDis = std::sqrt(std::pow(pPortal->GetX() - x, 2) + std::pow(pPortal->GetY() - y, 2));
		if (nIdx < 0 || (calcDis < dDis))
		{
			nIdx = index;
			dDis = calcDis;
		}
	}
	return m_apPortal[nIdx];
}

Portal* PortalMap::FindPortal(const std::string & sName)
{
	for (auto pPortal : m_apPortal)
	{
		if (pPortal->GetPortalName() == sName)
			return pPortal;
	}
	return nullptr;
}

Portal* PortalMap::GetRandStartPoint()
{
	if (m_aStartPoint.size() == 0)
		return nullptr;
	return m_apPortal[m_aStartPoint[rand() % m_aStartPoint.size()]];
}

bool PortalMap::IsPortalNear(const std::list<FieldPoint>& aptRoute, int nXrange)
{
	int nPX, nPY, nYRange = 70;
	for (auto pPortal : m_apPortal)
	{
		if (pPortal->GetPortalType() && pPortal->GetPortalType() != 6)
		{
			nPX = pPortal->GetX();
			nPY = pPortal->GetY();
			for (auto& point : aptRoute)
			{
				if ((point.x >= nPX - nXrange && point.x <= nPX + nXrange)
				 && (point.y >= nPY - nYRange && point.y <= nPY + nYRange))
					return true;
			}
		}
	}
	return false;
}

void PortalMap::ResetPortal()
{
	for (auto pPortal : m_apPortal)
		pPortal->SetEnable((pPortal->GetPortalType() == 4 || pPortal->GetPortalType() == 5));
}
