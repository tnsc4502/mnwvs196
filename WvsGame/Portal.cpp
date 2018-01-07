#include "Portal.h"


Portal::Portal(int nID, int nType, int nTargetMap, int nX, int nY, const std::string & sName, const std::string & sTName, const std::string & sScript)
	: m_nType(nType),
	  m_nTMap(nTargetMap),
	  m_nX(nX),
	  m_nY(nY),
	  m_nID(nID),
	  m_sName(sName),
	  m_sTName(sTName),
	  m_sScript(sScript),
	  m_bIsEnable(true)
{
}

Portal::~Portal()
{
}

int Portal::GetPortalType() const
{
	return m_nType;
}

int Portal::GetTargetMap() const
{
	return m_nTMap;
}

int Portal::GetX()
{
	return m_nX;
}

int Portal::GetY()
{
	return m_nY;
}

int Portal::GetID()
{
	return m_nID;
}

void Portal::SetX(int x)
{
	m_nX = x;
}

void Portal::SetY(int y)
{
	m_nY = y;
}

void Portal::SetID(int nID)
{
	m_nID = nID;
}

const std::string & Portal::GetPortalName() const
{
	return m_sName;
}

const std::string & Portal::GetPortalScriptName() const
{
	return m_sScript;
}

const std::string & Portal::GetTargetPortalName() const
{
	return m_sTName;
}

void Portal::SetPortalName(const std::string & ptName)
{
	m_sName = ptName;
}

void Portal::SetPortalScriptName(const std::string & ptScriptName)
{
	m_sScript = ptScriptName;
}

void Portal::SetTargetPortalName(const std::string & ptTargetPortalName)
{
	m_sTName = ptTargetPortalName;
}

bool Portal::IsEnable() const
{
	return m_bIsEnable;
}

void Portal::SetEnable(bool enable)
{
	m_bIsEnable = enable;
}
