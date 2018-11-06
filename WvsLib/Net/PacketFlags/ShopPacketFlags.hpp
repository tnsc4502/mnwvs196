#pragma once
#ifndef SHOP_FLAG
#define SHOP_FLAG

#include "PacketFlags.hpp"

#define MAKE_SHOP_INTERNAL_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static ShopInternalPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_SHOP_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static ShopRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_SHOP_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static ShopSendPacketFlag flagName {value, #flagName}; } const static int flagName = value

//==================================SHOP SEND===============================
MAKE_FLAG_COLLECTION_BODY(ShopSendPacketFlag)
MAKE_SHOP_SEND_PACKET_FLAG(Client_ValidateState, 0x14); //Stage::OnSetCashShop
MAKE_SHOP_SEND_PACKET_FLAG(Client_SetCashShop, 0x1C4); //Stage::OnSetCashShop
MAKE_SHOP_SEND_PACKET_FLAG(User_QueryCashResult, 0x5B6);
MAKE_SHOP_SEND_PACKET_FLAG(User_CashItemResult, 0x5B7);

FLAG_COLLECTION_BODY_END


//==================================SHOP RECV===============================
MAKE_FLAG_COLLECTION_BODY(ShopRecvPacketFlag)
MAKE_SHOP_RECV_PACKET_FLAG(User_OnQueryCashReques, 0x45E); 
MAKE_SHOP_RECV_PACKET_FLAG(User_OnCashItemRequest, 0x45F); 
MAKE_SHOP_RECV_PACKET_FLAG(User_OnCouponRequest, 0x460);
FLAG_COLLECTION_BODY_END


//==================================SHOP INTERNAL===============================
MAKE_FLAG_COLLECTION_BODY(ShopInternalPacketFlag)
MAKE_SHOP_INTERNAL_PACKET_FLAG(RequestMigrateIn, 0x7001);
MAKE_SHOP_INTERNAL_PACKET_FLAG(RequestTransferToGame, 0x7002);
MAKE_SHOP_INTERNAL_PACKET_FLAG(RequestMigrateOut, 0x7003);
MAKE_SHOP_INTERNAL_PACKET_FLAG(RequestLoadLocker, 0x7004);
MAKE_SHOP_INTERNAL_PACKET_FLAG(RequestUpdateCash, 0x7005);
MAKE_SHOP_INTERNAL_PACKET_FLAG(RequestMoveLToS, 0x7006); //Move To Slot From Locker
MAKE_SHOP_INTERNAL_PACKET_FLAG(RequestMoveSToL, 0x7007); //Move To Locker From Slot
MAKE_SHOP_INTERNAL_PACKET_FLAG(RequestBuyCashItem, 0x7008);

MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterLoadLockerDone, 0x1000);
MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterLoadLockerFailed, 0x1001);
MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterUpdateCashDone, 0x1002);
MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterUpdateCashFailed, 0x1003);
MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterMoveToSlotDone, 0x1004);
MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterMoveToSlotFailed, 0x1005);
MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterMoveToLockerDone, 0x1006);
MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterMoveToLockerFailed, 0x1007);
MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterResBuyDone, 0x1008);
MAKE_SHOP_INTERNAL_PACKET_FLAG(OnCenterResBuyFailed, 0x1009);

FLAG_COLLECTION_BODY_END

#endif