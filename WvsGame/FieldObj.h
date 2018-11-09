#pragma once
#include "FieldPoint.h"

struct MovePath;
class OutPacket;
class Field;

class FieldObj
{
protected:
	int m_nF, m_nFh, m_nCy, m_nRx0, m_nRx1, m_nHide, m_nFieldObjectID, m_nTemplateID;

	char m_bMoveAction;

	Field* m_pField = nullptr;

	FieldPoint m_ptPos;

public:
	FieldObj();
	~FieldObj();

	virtual void MakeEnterFieldPacket(OutPacket *oPacket) = 0;
	virtual void MakeLeaveFieldPacket(OutPacket *oPacket) = 0;

	void ValidateMovePath(MovePath *pMovePath);

	/*
	Basic Attributes
	*/
	void SetFieldObjectID(int objID);
	int GetFieldObjectID() const;

	void SetTemplateID(int templateID);
	int GetTemplateID() const;

	void SetPosX(int x);
	int GetPosX() const;

	void SetPosY(int y);
	int GetPosY() const;

	void SetF(int f);
	int GetF() const;

	void SetFh(int fh);
	int GetFh() const;

	void SetCy(int cy);
	int GetCy() const;

	void SetRx0(int rx0);
	int GetRx0() const;

	void SetRx1(int rx1);
	int GetRx1() const;

	void SetHide(int hide);
	bool IsHidden() const;

	char GetMoveAction() const;
	void SetMoveAction(char moveAction);
	virtual void SetMovePosition(int x, int y, bool bMoveAction, short nSN) {}

	Field* GetField();
	void SetField(Field *pField);
};

