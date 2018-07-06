#pragma once
#ifndef MOB_FLAG
#define MOB_FLAG

#include "PacketFlags.hpp"

#define MAKE_MOB_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static MobSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_MOB_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static MobRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(MobSendPacketFlag)
//以下開始註冊封包Opcode Flag
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMakeEnterFieldPacket, 0x3C1);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMakeLeaveFieldPacket, 0x3C2);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobChangeController, 0x3C3);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobCrcKeyChanged, 0x3C4);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobSetAfterAttack, 0x3C5);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobBlockAttack, 0x3D3);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMove, 0x3C7);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnCtrlAck, 0x3C8);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnStatSet, 0x3CA);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnStatReset, 0x3CB);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnAffected, 0x3CD);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnDamaged, 0x3CE);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnHPIndicator, 0x3D5);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnCatchEffect, 0x3D6);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnStealEffect, 0x3D7);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnEffectByItem, 0x3D8);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnSpecialEffectBySkill, 0x3D1);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnSuspendReset, 0x3CC);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobAttackedByMob, 0x40C);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobSpeaking, 0x3D9);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobMessaging, 0x3DA);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobSkillDelay, 0x3DB);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnEscortFullPath, 0x3DC);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnEscortStopEndPermmision, 0x3DD);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnEscortStopByScript, 0x3DE);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnEscortStopSay, 0x3DF);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnEscortReturnBefore, 0x3E0);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobTeleportRequest, 0x3E2);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnNextAttack, 0x3E1);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnForcedAction, 0x3E3);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnForcedSkillAction, 0x3E4);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnTimeResist, 0x3E6);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnOnekillDamage, 0x3E7);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnAttackBlock, 0x3E8);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnAttackPriority, 0x3E9);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnAttackTimeInfo, 0x3EA);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMoveAreaSet, 0x3EE);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnDamageShareInfoToLocal, 0x3EB);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnDamageShareInfoToRemote, 0x3EC);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnBreakDownTimeZoneTimeOut, 0x3ED);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnDoSkillByHit, 0x3EF);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnFlyTarget, 0x3F1);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnCastingBarSkill, 0x3F0);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnBounceAttackSkill, 0x3F2);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnAreaInstallByHit, 0x3F3);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnLtrbDamageSkill, 0x3F4);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnSummonSubBody, 0x3FA);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnLaserControl, 0x3FE);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnScale, 0x3FF);
MAKE_MOB_SEND_PACKET_FLAG(Mob_ForceChase, 0x402);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnHangOverRequest, 0x403);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnHangOverReleaseRequest, 0x404);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnDeadFPSMode, 0x405);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnAirHit, 0x406);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnDemianDelayedAttackCreate, 0x407);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnCrcDataRequest, 0x3D4);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnRegisterRelMobZone, 0x408);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnUnregisterRelMobZone, 0x409);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnNextTargetFromSvr, 0x40A);

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END



MAKE_FLAG_COLLECTION_BODY(MobRecvPacketFlag)
//以下開始註冊封包Opcode Flag

MAKE_MOB_RECV_PACKET_FLAG(Mob_OnMove, 0x369); //Mob::GenerateMovePath
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnApplyControl, 0x36A); //Mob::ApplyControl
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnDropPickUpRequest, 0x36B); //Mob::SendDropPickUpReques
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnDamagedByObstacleAtom, 0x36D); //Mob::SetDamagedByObstacleAtom
//0x36C & 0x36E used in unknown func (same func)
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnTryFirstSelfDestruction, 0x36F); //Mob::TryFirstSelfDestruction
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnTrySelfDestructionCollisionGroup, 0x370); //Mob::TrySelfDestructionCollisionGroup
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnDamagedByMob, 0x371); //Mob::SetDamagedByMob
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnUpdateTimeBomb, 0x373); //Mob::UpdateTimeBomb
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnCollisionEscort, 0x374); //Mob::SendCollisionEscort
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnRequestEscortPath, 0x375); //Mob::SendRequestEscortPath
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnEscortStopEndRequest, 0x376); //Mob::SendEscortStopEndRequest
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnAttack_Area, 0x377); //Mob::DoAttack_Area
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnExplosionStart, 0x378); //Mob::SendExplosionStart
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnInitMobUpDownState, 0x379); //Mob::InitMobUpDownState
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnUpdateFixedMoveDir, 0x37A); //Mob::UpdateFixedMoveDir
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnUpdatePassiveSkill, 0x37B); //Mob::UpdatePassiveSkill
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnRequsetAfterDead, 0x37C); //Mob::SendRequsetAfterDead
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnDamageShareInfoUpdate, 0x37D); //Mob::DamageShareInfoUpdate
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnCreateAffectedArea, 0x37E); //Mob::CreateAffectedArea

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#endif