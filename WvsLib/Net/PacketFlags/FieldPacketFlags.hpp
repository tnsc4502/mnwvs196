#pragma once
#ifndef FIELD_FLAG
#define FIELD_FLAG

#include "PacketFlags.hpp"

#define MAKE_FIELD_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static FieldSendPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(FieldSendPacketFlag)
//以下開始註冊封包Opcode Flag
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnTransferFieldReqIgnored, 0x1C5);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnTransferChannelReqIgnored, 0x1C6);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnTransferPvpReqIgnored, 0x1C7);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFieldSpecificData, 0x1C8);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnGroupMessage, 0x1C9);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFieldUniverseMessage, 0x1CA);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnWhisper, 0x1CB);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSummonItemInavailable, 0x1CC);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFieldEffect, 0x1CD);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnBlowWeather, 0x1CE);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnPlayJukeBox, 0x1CF);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnAdminResult, 0x1D1);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnQuiz, 0x1D2);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnDesc, 0x1D3);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnClock, 0x1D4);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetQuestClear, 0x1D7);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetQuestTime, 0x1D8);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetObjectState, 0x1D9);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnDestroyClock, 0x1DA);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnStalkResult, 0x1DC);
//QuickslotKeyMappedMan::OnInit, 0x1DF);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFootHoldMove, 0x1E0);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnCorrectFootHoldMove, 0x1E1);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSmartMobNotice, 0x1E6);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnChangePhase, 0x1E7);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnChangeMobZone, 0x1E8);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFootHoldOnOff, 0x1FE);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnLadderRopeOnOff, 0x1FF);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnMomentAreaOnOff, 0x200);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnMomentAreaOnOffAll, 0x201);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnPvPMigrateInfoResult, 0x1EA);
//InGameDirectionNodeInfo::OnCurNodeEventEnd, 0x1EB);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnCreateForceAtom, 0x1EC);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetAchieveRate, 0x1EE);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetQuickMoveInfo, 0x1EF);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnChangeAswanSiegeWeaponGauge, 0x1F0);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnCreateObtacle, 0x1F1);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnClearObtacle, 0x1F2);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnDebuffObjON, 0x1F5);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnCreateFallingCatcher, 0x1F8);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnChaseEffectSet, 0x1F9);
//User::OnMesoExchangeResult, 0x1FA);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetMirrorDungeonInfo, 0x1FB);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetIntrusion, 0x1FC);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnCannotDrop, 0x1FD);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnChatLetClientConnect, 0x202);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnChatInduceClientConnect, 0x203);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnEliteState, 0x204);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnPlaySound, 0x205);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnStackEventGauge, 0x206);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetUnionField, 0x20B);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnStarPlanetBurningTimeInfo, 0x20D);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnPublicShareState, 0x20E);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFunctionTempBlock, 0x20F);
//UIStatusBar::OnPacket, 0x210);
//FieldDelaySkill::OnFieldSkillDelay, 0x211);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnWeatherPacket_Add, 0x212);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnWeatherPacket_Remove, 0x213);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnWeatherPacket_Msg, 0x214);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnAddWreckage, 0x215);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnDelWreckage, 0x216);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnCreateMirrorImage, 0x217);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFuntionFootholdMan, 0x218);


													//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#endif