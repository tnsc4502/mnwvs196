#include "FieldObj.h"

FieldObj::FieldObj()
{
}


FieldObj::~FieldObj()
{
}

/*
Basic Attributes
*/

void FieldObj::SetFieldObjectID(int objID)
{
	nFieldObjectID = objID;
}

int FieldObj::GetFieldObjectID() const
{
	return nFieldObjectID;
}

void FieldObj::SetTemplateID(int templateID)
{
	nTemplateID = templateID;
}

int FieldObj::GetTemplateID() const
{
	return nTemplateID;
}

void FieldObj::SetPosX(int x)
{
	nPosX = x;
}

int FieldObj::GetPosX() const
{
	return nPosX;
}

void FieldObj::SetPosY(int y)
{
	nPosY = y;
}

int FieldObj::GetPosY() const
{
	return nPosY;
}

void FieldObj::SetF(int f)
{
	nF = f;
}

int FieldObj::GetF() const
{
	return nF;
}

void FieldObj::SetFh(int fh)
{
	nFh = fh;
}

int FieldObj::GetFh() const
{
	return nFh;
}

void FieldObj::SetCy(int cy)
{
	nCy = cy;
}

int FieldObj::GetCy() const
{
	return nCy;
}

void FieldObj::SetRx0(int rx0)
{
	nRx0 = rx0;
}

int FieldObj::GetRx0() const
{
	return nRx0;
}

void FieldObj::SetRx1(int rx1)
{
	nRx1 = rx1;
}

int FieldObj::GetRx1() const
{
	return nRx1;
}

void FieldObj::SetHide(int hide)
{
	nHide = hide;
}

bool FieldObj::IsHidden() const
{
	return nHide != 0;
}

char FieldObj::GetMoveAction() const
{
	return m_bMoveAction;
}

void FieldObj::SetMoveAction(char moveAction)
{
	m_bMoveAction = moveAction;
}

void FieldObj::SetMp(long long int mp)
{
	m_nMp = mp;
}

void FieldObj::SetHp(long long int hp)
{
	m_nHp = hp;
}

long long int FieldObj::GetMp() const
{
	return m_nMp;
}

long long int FieldObj::GetHp() const
{
	return m_nHp;
}

Field * FieldObj::GetField()
{
	return m_pField;
}

void FieldObj::SetField(Field * pField)
{
	m_pField = pField;
}
