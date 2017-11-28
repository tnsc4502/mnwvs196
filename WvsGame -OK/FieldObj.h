#pragma once

class OutPacket;

class FieldObj
{
	int nPosX, nPosY, nF, nFh, nCy, nRx0, nRx1, nHide, nFieldObjectID, nTemplateID;
public:
	FieldObj();
	~FieldObj();

	virtual void MakeEnterFieldPacket(OutPacket *oPacket) = 0;

	/*
	Basic Attributes
	*/
	void SetFieldObjectID(int objID)
	{
		nFieldObjectID = objID;
	}

	int GetFieldObjectID() const
	{
		return nFieldObjectID;
	}

	void SetTemplateID(int templateID)
	{
		nTemplateID = templateID;
	}

	int GetTemplateID() const
	{
		return nTemplateID;
	}

	void SetPosX(int x)
	{
		nPosX = x;
	}

	int GetPosX() const
	{
		return nPosX;
	}

	void SetPosY(int y)
	{
		nPosY = y;
	}

	int GetPosY() const
	{
		return nPosY;
	}

	void SetF(int f)
	{
		nF = f;
	}

	int GetF() const
	{
		return nF;
	}

	void SetFh(int fh)
	{
		nFh = fh;
	}

	int GetFh() const
	{
		return nFh;
	}

	void SetCy(int cy)
	{
		nCy = cy;
	}

	int GetCy() const
	{
		return nCy;
	}

	void SetRx0(int rx0)
	{
		nRx0 = rx0;
	}

	int GetRx0() const
	{
		return nRx0;
	}

	void SetRx1(int rx1)
	{
		nRx1 = rx1;
	}

	int GetRx1() const
	{
		return nRx1;
	}

	void SetHide(int hide)
	{
		nHide = hide;
	}

	bool IsHidden() const
	{
		return nHide != 0;
	}
};

