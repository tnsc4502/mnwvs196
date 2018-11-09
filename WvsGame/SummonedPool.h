#pragma once
#include "FieldPoint.h"

#include <vector>
#include <mutex>
#include <atomic>

class Field;
class User;
class Summoned;

class SummonedPool
{
	Field* m_pField;
	std::mutex m_mtxSummonedLock;
	std::atomic<int> m_nSummonedIdCounter;
	std::vector<Summoned*> m_lSummoned;

public:
	SummonedPool(Field *pField);
	~SummonedPool();

	std::mutex& GetSummonedPoolLock();
	Summoned* GetSummoned(int nFieldObjID);
	bool CreateSummoned(User* pUser, Summoned* pSummoned, const FieldPoint& pt);
	Summoned* CreateSummoned(User* pUser, int nSkillID, int nSLV, const FieldPoint& pt);
	void RemoveSummoned(int nCharacterID, int nSkillID, int nLeaveType);
};

