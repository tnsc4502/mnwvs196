#pragma once
#ifndef USER_FLAG
#define USER_FLAG

#include "PacketFlags.hpp"

#define MAKE_USER_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static UserPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(UserPacketFlag)
//以下開始註冊封包Opcode Flag

MAKE_USER_PACKET_FLAG(UserCommon_OnChat, 0x21E);

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#endif