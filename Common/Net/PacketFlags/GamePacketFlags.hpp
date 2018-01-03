#pragma once
#ifndef GAME_FLAG
#define GAME_FLAG

#include "PacketFlags.hpp"

#define MAKE_GAME_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static GamePacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(GamePacketFlag)
//以下開始註冊封包Opcode Flag

//Flags to Center
MAKE_GAME_PACKET_FLAG(RequestMigrateIn, 0x6001);
MAKE_GAME_PACKET_FLAG(RequestMigrateOut, 0x6002);


//Flags to Game
MAKE_GAME_PACKET_FLAG(ClientMigrateIn, 0x1BF);

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#endif