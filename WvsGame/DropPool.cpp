#include "DropPool.h"
#include "Drop.h"
#include "Reward.h"
#include "ItemInfo.h"
#include "Field.h"
#include "User.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Common\DateTime\GameDateTime.h"

DropPool::DropPool(Field *pField)
	: m_pField(pField)
{
	m_tLastExpire = GameDateTime::GetNowTime();
}


DropPool::~DropPool()
{
}

void DropPool::Create(Reward * reward, unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, unsigned int dwSourceID, int x1, int y1, int x2, int y2, int tDelay, int bAdmin, int nPos, bool bByPet)
{
	std::lock_guard<std::mutex> dropPoolock(m_mtxDropPoolLock);
	Drop *pDrop = new Drop();
	pDrop->Init(++m_nDropIdCounter, reward, dwOwnerID, dwOwnPartyID, nOwnType, dwSourceID, x1, y1, x2, y2, bByPet);
	auto pItem = pDrop->GetItem();
	if (reward->GetType() == 1 && reward->GetPeriod() != 0)
		pItem->liExpireDate = GameDateTime::GetDateExpireFromPeriod(reward->GetPeriod());
	bool bEverLasting = m_bDropEverlasting ? dwSourceID == 0 : false;
	if (bAdmin)
		bEverLasting = reward->GetType() == 1
		&& (ItemInfo::GetInstance()->IsQuestItem(pItem->nItemID) || ItemInfo::GetInstance()->IsTradeBlockItem(pItem->nItemID));

	if (reward->GetType() == 1
		&& !dwSourceID
		&& !bAdmin
		&& (ItemInfo::GetInstance()->IsQuestItem(pItem->nItemID) || ItemInfo::GetInstance()->IsTradeBlockItem(pItem->nItemID)))
	{
		//丟出後立即消失
		OutPacket oPacket;
		pDrop->MakeEnterFieldPacket(&oPacket, 3, tDelay);
		if (!dwOwnerID)
		{
			delete pItem;
			pDrop->m_pItem = nullptr;
		}
		m_pField->BroadcastPacket(&oPacket);
	}
	else
	{
		OutPacket oPacket;
		pDrop->MakeEnterFieldPacket(&oPacket, 1, tDelay);
		m_pField->BroadcastPacket(&oPacket);
		OutPacket oPacket2;
		pDrop->MakeEnterFieldPacket(&oPacket2, 0, tDelay);
		m_pField->BroadcastPacket(&oPacket2);
	}
	m_mDrop[pDrop->m_dwDropID] = pDrop;
}

void DropPool::OnEnter(User * pUser)
{
	std::lock_guard<std::mutex> dropPoolock(m_mtxDropPoolLock);
	for (auto& drop : m_mDrop)
	{
		OutPacket oPacket;
		drop.second->MakeEnterFieldPacket(&oPacket);
		pUser->SendPacket(&oPacket);
	}
}
