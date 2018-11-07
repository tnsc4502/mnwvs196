#pragma once
#include "FieldObj.h"

struct GW_ItemSlotPet;
class InPacket;
class Field;

class Pet : public FieldObj
{
	friend class User;

	User *m_pOwner;
	Field *m_pField;
	GW_ItemSlotPet *m_pPetSlot;
	unsigned char m_nIndex = 0;

public:
	Pet(GW_ItemSlotPet *pPetSlot);
	~Pet();

	void SetIndex(unsigned char nIndex);
	unsigned char GetIndex() const;

	void Init(User *pUser);
	void OnPacket(InPacket *iPacket);
	void OnEnterField(Field *pField);
	void OnLeaveField();
	void OnMove(InPacket *iPacket);
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
};

