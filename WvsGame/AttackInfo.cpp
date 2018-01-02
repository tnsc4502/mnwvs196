#include "AttackInfo.h"

int AttackInfo::GetDamagedMobCount()
{
	return (this->m_bAttackInfoFlag >> 4 & 0xF);
}

int AttackInfo::GetDamageCountPerMob()
{
	return this->m_bAttackInfoFlag & 0xF;
}
