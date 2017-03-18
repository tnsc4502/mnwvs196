#pragma once
#ifndef LOGIN_FLAG
#define LOGIN_FLAG

#include "PacketFlags.hpp"

#define MAKE_LOGIN_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static LoginPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(LoginPacketFlag) 
//以下開始註冊封包Opcode Flag

//Flags for Center
MAKE_LOGIN_PACKET_FLAG(RegisterCenterRequest, 1);


//Flags for Client

MAKE_LOGIN_PACKET_FLAG(LoginBackgroundResponse, 0x2A);
MAKE_LOGIN_PACKET_FLAG(ClientStartResponse, 0x2F);

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#endif