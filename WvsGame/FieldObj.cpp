#include "FieldObj.h"
#include "MovePath.h"

FieldObj::FieldObj()
{
}


FieldObj::~FieldObj()
{
}

void FieldObj::ValidateMovePath(MovePath * pMovePath)
{
	auto& movePath = *pMovePath;
	for (auto& elem : movePath.m_lElem)
	{
		m_ptPos.x = elem.x;
		m_ptPos.y = elem.y;
		SetMoveAction(elem.bMoveAction);
		SetFh(elem.fh);
	}
}

/*
Basic Attributes
*/

void FieldObj::SetFieldObjectID(int objID)
{
	m_nFieldObjectID = objID;
}

int FieldObj::GetFieldObjectID() const
{
	return m_nFieldObjectID;
}

void FieldObj::SetTemplateID(int templateID)
{
	m_nTemplateID = templateID;
}

int FieldObj::GetTemplateID() const
{
	return m_nTemplateID;
}

void FieldObj::SetPosX(int x)
{
	m_ptPos.x = x;
}

int FieldObj::GetPosX() const
{
	return m_ptPos.x;
}

void FieldObj::SetPosY(int y)
{
	m_ptPos.y = y;
}

int FieldObj::GetPosY() const
{
	return m_ptPos.y;
}

void FieldObj::SetF(int f)
{
	m_nF = f;
}

int FieldObj::GetF() const
{
	return m_nF;
}

void FieldObj::SetFh(int fh)
{
	m_nFh = fh;
}

int FieldObj::GetFh() const
{
	return m_nFh;
}

void FieldObj::SetCy(int cy)
{
	m_nCy = cy;
}

int FieldObj::GetCy() const
{
	return m_nCy;
}

void FieldObj::SetRx0(int rx0)
{
	m_nRx0 = rx0;
}

int FieldObj::GetRx0() const
{
	return m_nRx0;
}

void FieldObj::SetRx1(int rx1)
{
	m_nRx1 = rx1;
}

int FieldObj::GetRx1() const
{
	return m_nRx1;
}

void FieldObj::SetHide(int hide)
{
	m_nHide = hide;
}

bool FieldObj::IsHidden() const
{
	return m_nHide != 0;
}

char FieldObj::GetMoveAction() const
{
	return m_bMoveAction;
}

void FieldObj::SetMoveAction(char moveAction)
{
	m_bMoveAction = moveAction;
}

Field * FieldObj::GetField()
{
	return m_pField;
}

void FieldObj::SetField(Field * pField)
{
	m_pField = pField;
}
