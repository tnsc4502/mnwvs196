#pragma once
#include <vector>
#include <list>
#include "FieldPoint.h"

class Portal;
class Field;

class PortalMap
{
private:
	std::vector<long> m_aStartPoint;
	std::vector<Portal*> m_apPortal;

public:
	PortalMap();
	~PortalMap();

	void RestorePortal(Field* pField, void* pPropPortal);
	void EnablePortal(const std::string& sName, bool bEnable);
	Portal* FindCloseStartPoint(int x, int y);
	Portal* FindPortal(const std::string& sName);
	Portal* GetRandStartPoint();
	bool IsPortalNear(const std::list<FieldPoint>& aptRoute, int nXrange);
	void ResetPortal();
};

