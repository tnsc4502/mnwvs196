#pragma once

class ClientSocket;
class OutPacket;
class InPacket;
class AsyncScheduler;
struct GA_Character;
struct GW_FuncKeyMapped;

class User
{
	enum CashItemPurchaseResult
	{
		Res_Success = 0x01,
		Res_Failed = 0x00
	};

	enum CashItemRequest
	{
		Recv_OnCashItemReqCoupon = 0x00,
		Recv_OnCashItemReqBuy = 0x03,
		Recv_OnCashItemReqGift = 0x04,
		Recv_OnCashItemReqIncItemSlot = 0x05,
		Recv_OnCashItemReqMoveItemToSlot = 0x0E,
		Recv_OnCashItemReqMoveItemToLocker = 0x0F,
		Recv_OnCashItemReqPackage = 0x24,

		Send_OnCashItemResLimitGoodsCountChanged =       0x02,
		Send_OnCashItemResLoadLockerDone =               0x06,
		Send_OnCashItemResLoadLockerFailed =             0x08,
		Send_OnCashItemResLoadGiftDone =                 0x09,
		Send_OnCashItemResLoadGiftFailed =               0x0A,
		Send_OnCashItemResLoadWishDone =                 0x0B,
		Send_OnCashItemResLoadWishFailed =               0x0C,
		Send_OnCashItemResSetWishDone =                  0x0D,
		Send_OnCashItemResSetWishFailed =                0x0E,
		Send_OnCashItemResBuyDone =                      0x0F,
		Send_OnCashItemResBuyFailed =                    0x10,
		Send_OnCashItemResUseCouponDone =                0x11,
		Send_OnCashItemResGiftCouponDone =               0x15,
		Send_OnCashItemResGiftCouponFaile =              0xFF1,
		Send_OnCashItemResGiftDone =                     0x18,
		Send_OnCashItemResGiftFailed =                   0x19,
		Send_OnCashItemResIncSlotCountDone =             0x1A,
		Send_OnCashItemResIncSlotCountFailed =           0x1B,
		Send_OnCashItemResIncTrunkCountDone =            0x1C,
		Send_OnCashItemResIncTrunkCountFailed =          0x1D,
		Send_OnCashItemResIncCharacterSlotCountDone =    0x1E,
		Send_OnCashItemResIncCharacterSlotCountFailed =  0x1F,
		Send_OnCashItemResIncBuyCharacterCountDone =     0x20,
		Send_OnCashItemResIncBuyCharacterCountFailed =   0x21,
		Send_OnCashItemResEnableEquipSlotExtDone =       0x22,
		Send_OnCashItemResEnableEquipSlotExtFailed =     0x23,
		Send_OnCashItemResMoveStoLDone =                 0x26,
		Send_OnCashItemResMoveStoLFailed =               0x27,
		Send_OnCashItemResMoveLtoSDone =                 0x24,
		Send_OnCashItemResMoveLtoSFailed =               0x25,
		Send_OnCashItemResDestroyDone =                  0x28,
		Send_OnCashItemResDestroyFailed =                0x29,
		Send_OnCashItemResExpireDone =                   0x2A,
		Send_OnCashItemResRebateDone =                   0x47,
		Send_OnCashItemResRebateFailed =                 0x48,
		Send_OnCashItemResCoupleDone =                   0x4B,
		Send_OnCashItemResCoupleFailed =                 0x4C,
		Send_OnCashItemResBuyPackageDone =               0x4D,
		Send_OnCashItemResBuyPackageFailed =             0x4E,
		Send_OnCashItemResGiftPackageDone =              0x4F,
		Send_OnCashItemResGiftPackageFailed =            0x50,
		Send_OnCashItemResBuyNormalDone =                0x51,
		Send_OnCashItemResBuyNormalFailed =              0x52,
		Send_OnCashItemResApplyWishListEventDone =       0x53,
		Send_OnCashItemResApplyWishListEvenFailed =      0x54,
		Send_OnCashItemResFriendShopDone =               0x55,
		Send_OnCashItemResFirendShopFailed =             0x56,
		Send_OnCashItemResPurchaseRecord =               0x66,
		Send_OnCashItemResPurchaseRecordFailed =         0x69,
	};

	int m_nChannelID, nNexonCash = 0, nMaplePoint = 0;
	ClientSocket *m_pSocket;
	GA_Character *m_pCharacterData;
	GW_FuncKeyMapped *m_pFuncKeyMapped;
	AsyncScheduler *m_pUpdateTimer;

public:
	User(ClientSocket *pSocket, InPacket *iPacket);
	~User();
	int GetUserID() const;
	static User * FindUser(int nUserID);
	void SendPacket(OutPacket *oPacket);
	void OnPacket(InPacket *iPacket);
	void OnMigrateOutCashShop();
	void Update();

	void OnUserCashItemRequest(InPacket *iPacket);
	void OnQueryCashRequest();
	void ValidateState();

	void OnCenterCashItemResult(int nType, InPacket *iPacket);
	void OnCenterResLoadLockerDone(InPacket *iPacket);
	void OnCenterResBuyDone(InPacket *iPacket);
	void OnCenterUpdateCashDone(InPacket *iPacket);
	void OnCenterMoveItemToSlotDone(InPacket *iPacket);
	void OnCenterMoveItemToLockerDone(InPacket *iPacket);

	void OnRequestCenterLoadLocker();
	void OnRequestCenterUpdateCash();
	void OnRequestBuyCashItem(InPacket *iPacket);
	void OnRequestMoveItemToSlot(InPacket *iPacket);
	void OnRequestMoveItemToLocker(InPacket *iPacket);
};