#pragma once

class OutPacket;
class Field;

class FieldObj
{
protected:
	int nPosX, nPosY, nF, nFh, nCy, nRx0, nRx1, nHide, nFieldObjectID, nTemplateID;
	long long int m_nMp, m_nHp;

	char m_bMoveAction;

	Field* m_pField;

public:
	FieldObj();
	~FieldObj();

	virtual void MakeEnterFieldPacket(OutPacket *oPacket) = 0;
	virtual void MakeLeaveFieldPacket(OutPacket *oPacket) = 0;

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

	void SetMp(long long int mp);
	void SetHp(long long int hp);
	long long int GetMp() const;
	long long int GetHp() const;

	Field* GetField();
	void SetField(Field *pField);
};

