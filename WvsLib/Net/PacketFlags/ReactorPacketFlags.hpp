#pragma once
#ifndef REACTOR_FLAG
#define REACTOR_FLAG

#include "PacketFlags.hpp"

#define MAKE_REACTOR_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static ReactorSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_REACTOR_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static ReactorRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(ReactorSendPacketFlag)

MAKE_REACTOR_SEND_PACKET_FLAG(Reactor_OnReactorChangeState, 0x441);
MAKE_REACTOR_SEND_PACKET_FLAG(Reactor_OnReactorEnterField, 0x443);
MAKE_REACTOR_SEND_PACKET_FLAG(Reactor_OnReactorLeaveField, 0x447);
MAKE_REACTOR_SEND_PACKET_FLAG(Reactor_OnReactorResetState, 0x444);
MAKE_REACTOR_SEND_PACKET_FLAG(Reactor_OnReactorOwnerInfo, 0x445);
MAKE_REACTOR_SEND_PACKET_FLAG(Reactor_OnReactorRemove, 0x446);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END


MAKE_FLAG_COLLECTION_BODY(ReactorRecvPacketFlag)

MAKE_REACTOR_RECV_PACKET_FLAG(Reactor_OnHitReactor, 0x38E);
MAKE_REACTOR_RECV_PACKET_FLAG(Reactor_OnClickReactor, 0x38F);
MAKE_REACTOR_RECV_PACKET_FLAG(Reactor_OnKey, 0x390);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END
#endif