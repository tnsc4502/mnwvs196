#pragma once

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
	static bool RawAddItemByID(User* pUser, int nItemID, int nCount);
	//static void RestoreMoneyFromTemp(User* pUser, bool bByPet, int nAmount);
};

