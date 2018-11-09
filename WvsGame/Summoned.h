#pragma once
#include "FieldObj.h"

class InPacket;
class OutPacket;
class User;
class SummonedPool;

class Summoned : public FieldObj
{
public:
	static int SUMMONED_MOV, SUMMONED_ATT;
	enum LeaveType
	{
		eLeave_ResetByTime = 0,
		eLeave_TransferField = 2,
		eLeave_UserDead = 3,
	};

private:
	friend class SummonedPool;
	User* m_pOwner;

	bool 
		m_bFlyMob = true, 
		m_bBeforeFirstAttack = true, 
		m_bMirrored = false,
		m_bJaguarActive = true,
		m_bAttackActive = true;

	int m_nSkillID = 0, 
		m_nSLV = 0,
		m_nMoveAbility = 0,
		m_nAssitType = 0,
		m_nEnterType = 0,
		m_nMobID = 0,
		m_nLookID = 0,
		m_nBulletID = 0,
		m_tEnd = 0;

public:
	Summoned();
	~Summoned();

	int GetOwnerID() const;
	int GetSkillID() const;
	void Init(User *pUser, int nSkillID, int nSLV);
	void OnPacket(InPacket *iPacket);
	void OnMove(InPacket *iPacket);
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	int GetMoveAbility();
	int GetAssitType();
};

