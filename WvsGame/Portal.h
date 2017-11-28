#pragma once
#include <iostream>

class Portal
{
private:
	int m_nType, //pt
		m_nTMap, //tm
		m_nX, //x
		m_nY; //y

	std::string m_sName, //pn
		m_sTName, //tn
		m_sScript; //script

	bool m_bIsEnable;
public:
	Portal(int nType, int nTargetMap, int nX, int nY, const std::string& sName, const std::string& sTName, const std::string& sScript);
	~Portal();

	int GetPortalType() const;
	int GetTargetMap() const;
	int GetX();
	int GetY();
	void SetX(int x);
	void SetY(int y);
	const std::string& GetPortalName() const;
	const std::string& GetPortalScriptName() const;
	const std::string& GetTargetPortalName() const;

	void SetPortalName(const std::string& ptName);
	void SetPortalScriptName(const std::string& ptScriptName);
	void SetTargetPortalName(const std::string& ptTargetPortalName);

	bool IsEnable() const;
	void SetEnable(bool enable);
};

