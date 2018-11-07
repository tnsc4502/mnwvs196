#pragma once

#include <vector>

struct BackupItem;
struct ExchangeElement;
struct GA_Character;
struct GW_ItemSlotBase;
class OutPacket;

class InventoryManipulator
{
public:
	enum ExchangeResult
	{
		Exchange_Success = 0,
		Exchange_InsufficientMeso = 1,
		Exchange_InsufficientSlotCount = 2,
		Exchange_InsufficientItemCount = 3
	};

	enum ChangeType
	{
		Change_AddToSlot = 0,
		Change_QuantityChanged = 1,
		Change_SlotPOSChanged = 2,
		Change_RemoveFromSlot = 3,
	};

	struct ChangeLog
	{
		GW_ItemSlotBase *pItem;
		int nChange, nTI, nPOS, nPOS2, nNumber;
	};

	InventoryManipulator();
	~InventoryManipulator();

	static void SwapSlot(GA_Character* pCharacterData, std::vector<ChangeLog>& aChangeLog, int nTI, int nPOS1, int nPOS2);
	static bool IsItemExist(GA_Character* pCharacterData, int nTI, int nItemID);

	static bool RawIncMoney(GA_Character* pCharacterData, int nMoney);

	//將pItem直接加入玩家背包
	static bool RawAddItem(GA_Character* pCharacterData, int nTI, GW_ItemSlotBase* pItem, std::vector<ChangeLog>& aChangeLog, int *nIncRet, bool bDeleteIfItemCombined, std::vector<BackupItem>* paBackupItem = nullptr);

	//給定nItemID, nCount加入玩家背包
	static bool RawAddItem(GA_Character* pCharacterData, int nTI, int nItemID, int nCount, std::vector<ChangeLog>& aChangeLog, int *nIncRet, std::vector<BackupItem>* paBackupItem = nullptr);
	static bool RawRemoveItem(GA_Character* pCharacterData, int nTI, int nPOS, int nCount, std::vector<ChangeLog>& aChangeLog, int *nDecRet, GW_ItemSlotBase **ppItemRemoved, std::vector<BackupItem>* paBackupItem = nullptr);
	static int RawExchange(GA_Character* pCharacterData, int nMoney, std::vector<ExchangeElement>& aExchange, std::vector<ChangeLog>& aLogAdd, std::vector<ChangeLog>& aLogRemove);

	//在Exchange失敗的時候進行恢復
	static void RestoreBackupItem(GA_Character* pCharacterData, std::vector<BackupItem>* paBackupItem);

	static void InsertChangeLog(std::vector<ChangeLog>& aChangeLog, int nChange, int nTI, int nPOS, GW_ItemSlotBase* pi, int nPOS2, int nNumber);
	static void MakeInventoryOperation(OutPacket *oPacket, int bOnExclResult, std::vector<InventoryManipulator::ChangeLog> &aChangeLog);
	static void MakeItemUpgradeEffect(OutPacket *oPacket, int nCharacterID, int nEItemID, int nUItemID, bool bSuccess, bool bCursed, bool bEnchant);
};

