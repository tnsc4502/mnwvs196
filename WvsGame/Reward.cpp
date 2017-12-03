#include "Reward.h"
#include "..\Database\GW_ItemSlotBase.h"


Reward::Reward()
{
}

Reward::Reward(Reward * pOther)
{
	m_pItem = pOther->m_pItem;
	m_nMoney = pOther->m_nMoney;
	m_nType = pOther->m_nType;
	m_pInfo = pOther->m_pInfo;
}


Reward::~Reward()
{
}
