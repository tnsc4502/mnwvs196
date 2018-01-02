#pragma once
#include <map>
#include <vector>

struct AttackInfo
{
	std::map<int, std::vector<long long int>> m_mDmgInfo;

	int m_bAttackInfoFlag;

	//How many monsters were damaged
	int GetDamagedMobCount();

	//How many count the monster was damaged
	int GetDamageCountPerMob();

	char 
		m_bCheckExJablinResult,
		m_bAddAttackProc,
		m_bFieldKey,
		m_bEvanForceAction;

	int 
		m_nX, 
		m_nY, 
		m_nType, 
		m_nAttackSpeed, 
		m_tLastAttackTime,
		m_nSoulArrow, 
		m_nWeaponType, 
		m_nShootRange, 
		m_nAttackActionType, 
		m_nDisplay,
		m_apMinion,
		m_nSkillID,
		m_nSLV,
		m_dwCRC,
		m_nSlot,
		m_nCsStar,
		m_tKeyDown,
		m_pGrenade,
		m_nFinalAttack;

};

