#pragma once
#ifndef MOB_FLAG
#define MOB_FLAG

#include "Net\PacketFlags\PacketFlags.hpp"

#define MAKE_USER_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static MobSendPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(MobSendPacketFlag)
//以下開始註冊封包Opcode Flag

MAKE_USER_PACKET_FLAG(MobToClient_SpawnMonster, 0x3C1);


//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#define MAKE_USER_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static MobRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(MobRecvPacketFlag)
//以下開始註冊封包Opcode Flag

MAKE_USER_PACKET_FLAG(MobToClient_SpawnMonster, 0x3C1);


//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#endif