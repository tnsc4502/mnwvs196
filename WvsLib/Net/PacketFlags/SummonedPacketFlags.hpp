#pragma once
#ifndef SUMMONED_FLAG
#define SUMMONED_FLAG

#include "PacketFlags.hpp"

#define MAKE_SUMMONED_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static SummonedSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_SUMMONED_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static SummonedRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(SummonedSendPacketFlag)
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnCreated, 0x3AF);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnRemoved, 0x3B0);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnMove, 0x3B1);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnAttack, 0x3B2);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnAttackPvP, 0x3B3);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnSetReference, 0x3B4);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnSkill, 0x3B5);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnSkillPvP, 0x3B6);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnUpdateHPTag, 0x3B7);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnAttackDone, 0x3B8);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnSetResist, 0x3B9);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnSummonedActionChange, 0x3BA);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnAssistAttackRequest, 0x3BB);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnSummonAttackActive, 0x3BC);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnSummonBeholderRevengeAttack, 0x3BD);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnHit, 0x3C0);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END

MAKE_FLAG_COLLECTION_BODY(SummonedRecvPacketFlag)
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnMoveRequest, 0x263);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnAttackRequest, 0x264);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnHitRequest, 0x265);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnDoingHealRequest, 0x266);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnRemoveRequest, 0x267);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnAttackForPvPRequest, 0x268);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnActionRequest, 0x269);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnAssistAttackRequest, 0x26A);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END
#endif