#pragma once
#include <vector>
#include "InventoryManipulator.h"
#include "ExchangeElement.h"

class User;
class InPacket;
struct GW_ItemSlotBase;

class QWUInventory
{
public:
	QWUInventory();
	~QWUInventory();
	static bool ChangeSlotPosition(User* pUser, int bOnExclRequest, int nTI, int nPOS1, int nPOS2, int nCount, int tRequestTime);
	static void OnChangeSlotPositionRequest(User* pUser, InPacket* iPacket);
	static bool PickUpMoney(User* pUser, bool byPet, int nAmount);
	static bool PickUpItem(User* pUser, bool byPet, GW_ItemSlotBase* pItem);
	static bool RawRemoveItemByID(User* pUser, int nItemID, int nCount);
	static bool RawRemoveItem(User* pUser, int nTI, int nPOS, int nCount, std::vector<InventoryManipulator::ChangeLog>& aChangeLog, int &nDecRet, GW_ItemSlotBase** ppItemRemoved);
	static bool RawAddItemByID(User* pUser, int nItemID, int nCount);
	static int Exchange(User* pUser, int nMoney, std::vector<ExchangeElement>& aExchange, std::vector<InventoryManipulator::ChangeLog>& aLogAdd, std::vector<InventoryManipulator::ChangeLog>& aLogRemove);
	static void SendInventoryOperation(User* pUser, int bOnExclResult, std::vector<InventoryManipulator::ChangeLog>& aChangeLog);
	static void OnUpgradeItemRequest(User* pUser, InPacket *iPacket);
	static void UpgradeEquip(User* pUser, int nUPOS, int nEPOS, int nWhiteScroll, bool bEnchantSkill, int tReqTime);

	static int GetSlotCount(User *pUser, int nTI);
	static int GetHoldCount(User *pUser, int nTI);
	static int GetFreeCount(User *pUser, int nTI);
	//static void RestoreMoneyFromTemp(User* pUser, bool bByPet, int nAmount);
};

