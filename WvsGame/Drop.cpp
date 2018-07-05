#include "Drop.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "Reward.h"
#include "User.h"

Drop::Drop()
{
}


Drop::~Drop()
{
}

void Drop::MakeEnterFieldPacket(OutPacket * oPacket)
{
	MakeEnterFieldPacket(oPacket, 2, 0);
}

void Drop::MakeEnterFieldPacket(OutPacket * oPacket, int nEnterType, int tDelay)
{
	oPacket->Encode2(0x430); //Drop Pool :: Enter Field
	oPacket->Encode1(0); //eDropType
	oPacket->Encode1(nEnterType);
	oPacket->Encode4(m_dwDropID);
	oPacket->Encode1(m_bIsMoney);
	oPacket->Encode4(0); //nDropMotionType
	oPacket->Encode4(0); //nDropSpeed
	oPacket->Encode4(0); //bNoMove
	oPacket->Encode4(m_pItem != nullptr ? m_pItem->nItemID : m_nMoney);
	oPacket->Encode4(m_dwOwnerID);
	oPacket->Encode1(m_nOwnType);
	oPacket->Encode2(m_pt2.x);
	oPacket->Encode2(m_pt2.y);
	oPacket->Encode4(m_dwSourceID);
	oPacket->Encode1(0); //unk 
	oPacket->Encode1(0); //unk
	oPacket->Encode4(0); //unk
	if (!nEnterType || nEnterType == 1 || nEnterType == 3)
	{
		oPacket->Encode2(m_pt1.x);
		oPacket->Encode2(m_pt1.y);
		oPacket->Encode4(tDelay);
	}
	oPacket->Encode1(0); //bExplosiveDrop
	//printf("MakeEnterFieldPacket Money ? %d\n", (int)m_nMoney);
	if (m_nMoney == 0)
		oPacket->Encode8(m_pItem->liExpireDate);
	oPacket->Encode1(m_bByPet);
	oPacket->Encode1(0);
	oPacket->Encode2(0); //nFallingVY
	oPacket->Encode1(0); //nFadeInEffect
	oPacket->Encode1(0); //nMakeType
	oPacket->Encode4(0); //bCollisionPickup
	oPacket->Encode1(0); // nItemGrade
	oPacket->Encode1(0); //bPrepareCollisionPickUp
	oPacket->Encode4(0); //unk
	oPacket->Encode1(0); //unk
}

void Drop::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	MakeLeaveFieldPacket(oPacket, 1, 0);
}

void Drop::MakeLeaveFieldPacket(OutPacket * oPacket, int nLeaveType, int nOption)
{
	oPacket->Encode2(0x432);
	oPacket->Encode1((char)nLeaveType);
	oPacket->Encode4(m_dwDropID);
	if (nLeaveType >= 2)
	{
		oPacket->Encode4(nOption);
		if (nLeaveType == 5)
			oPacket->Encode4(0);
	}
}

bool Drop::IsShowTo(User * pUser)
{
	return true;
}

void Drop::Init(unsigned int dwDropID, Reward * reward, unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, unsigned int dwSourceID, int x1, int y1, int x2, int y2, int bByPet)
{
	this->m_dwDropID = dwDropID;
	this->m_dwOwnerID = dwOwnerID;
	this->m_dwOwnPartyID = dwOwnPartyID;
	this->m_nOwnType = nOwnType;
	this->m_dwSourceID = dwSourceID;
	this->m_bByPet = bByPet != 0;
	this->m_pt1.x = x1;
	this->m_pt1.y = y1;
	this->m_pt2.x = x2;
	this->m_pt2.y = y2;
	this->m_bIsMoney = reward->GetMoney() != 0;
	this->m_nMoney = reward->GetMoney();
	this->m_usQRKey = 0;
	this->m_nShowMax = 0;
	this->m_nPeriod = reward->GetPeriod();
	auto pInfo = reward->GetRewardInfo();
	if (pInfo)
	{
		m_usQRKey = pInfo->m_usQRKey;
		m_nShowMax = pInfo->m_nMaxCount;
	}
	m_pItem = reward->GetItem();
}

GW_ItemSlotBase* Drop::GetItem()
{
	return m_pItem;
}

int Drop::GetDropInfo() const
{
	if (m_nMoney > 0)
		return m_nMoney;
	if (m_pItem)
		return m_pItem->nItemID;
	return 0;
}
