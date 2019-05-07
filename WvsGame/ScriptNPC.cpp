#include "ScriptNPC.h"
#include "Script.h"
#include "User.h"
#include "QWUser.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\WvsLib\Script\luaconf.h"
#include "..\WvsLib\Script\lvm.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\NPCPacketFlags.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

ScriptNPC::ScriptNPC()
{
}


ScriptNPC::~ScriptNPC()
{
}

ScriptNPC * ScriptNPC::GetSelf(lua_State * L)
{
	auto pSys =((Script*)L->selfPtr);
	if (pSys->m_pUniqueScriptNpc == nullptr)
		pSys->m_pUniqueScriptNpc = AllocObj(ScriptNPC);
	return (ScriptNPC*)pSys->m_pUniqueScriptNpc;
}

void ScriptNPC::DestroySelf(lua_State * L, ScriptNPC * p)
{
	FreeObj(p);
}

void ScriptNPC::Register(lua_State * L)
{
	luaL_Reg SelfMetatable[] = {
		{ "askAvatar", SelfAskAvatar },
		{ "askText", SelfAskText },
		{ "askYesNo", SelfAskYesNo },
		{ "askAccept", SelfAskAcceptDecline },
		{ "askNumber", SelfAskNumber },
		{ "askMenu", SelfAskMenu },
		{ "sayNext", SelfSayNext },
		{ "say", SelfSay },
		{ "debug", Debug },
		{ NULL, NULL }
	};

	luaL_Reg SelfTable[] = {
		{ NULL, NULL }
	};
	lua_pushinteger(L, ScriptStyle::ePlayerTalk);
	lua_setglobal(L, "style_playerTalk");
	lua_pushinteger(L, ScriptStyle::ePlayerRespond);
	lua_setglobal(L, "style_playerRespond");
	lua_pushinteger(L, ScriptStyle::eFullScreen);
	lua_setglobal(L, "style_fullScreen");
	lua_pushinteger(L, ScriptStyle::eInverseDirection);
	lua_setglobal(L, "style_inverseDirection");
	lua_pushinteger(L, ScriptStyle::eNoESC);
	lua_setglobal(L, "style_noESC");

	luaW_register<ScriptNPC>(L,
		"Npc", 
		SelfTable, 
		SelfMetatable, 
		&(ScriptNPC::GetSelf),
		&(ScriptNPC::DestroySelf));
}

void ScriptNPC::OnPacket(InPacket * iPacket, Script * pScript, lua_State* L)
{
	char nMsgType = iPacket->Decode1(), nAction = 0;
	if (nMsgType != ScriptType::OnSay || pScript->GetConverstaionState().m_bPaging == false) 
	{
		pScript->GetConverstaionState().m_aPageStack.clear();
		pScript->GetConverstaionState().m_nCurPage = 0;
	}

	switch (nMsgType)
	{
		case ScriptType::OnSay:
		{
			nAction = iPacket->Decode1();
			if (nAction == (char)0xFF)
				pScript->Abort();
			else if (nAction == 0) 
			{
				pScript->GetConverstaionState().m_nUserInput = 0;
				if (pScript->GetConverstaionState().m_bPaging 
					&& pScript->GetConverstaionState().m_nCurPage != 1)
				{
					SelfSayNextImpl(L, --(pScript->GetConverstaionState().m_nCurPage) - 1);
					return;
				}
			}
			else if (nAction == 1) 
			{
				pScript->GetConverstaionState().m_nUserInput = 1;
				if (pScript->GetConverstaionState().m_bPaging
					&& pScript->GetConverstaionState().m_nCurPage != pScript->GetConverstaionState().m_aPageStack.size())
				{
					SelfSayNextImpl(L, ++(pScript->GetConverstaionState().m_nCurPage) - 1);
					return;
				}
			}
			break;
		}
		case ScriptType::OnAskYesNo:
		{
			nAction = iPacket->Decode1();
			if (nAction == (char)0xFF)
				pScript->Abort();
			else if (nAction == 0)
				pScript->GetConverstaionState().m_nUserInput = 0;
			else if (nAction == 1)
				pScript->GetConverstaionState().m_nUserInput = 1;
			lua_pushinteger(L, pScript->GetConverstaionState().m_nUserInput);
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
		case ScriptType::OnAskText:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0)
				pScript->Abort();
			else if (nAction == 1)
				lua_pushstring(L, iPacket->DecodeStr().c_str());
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
		case ScriptType::OnAskNumber:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0)
				pScript->Abort();
			else if (nAction == 1)
				pScript->GetConverstaionState().m_nUserInput = iPacket->Decode4();
			lua_pushinteger(L, pScript->GetConverstaionState().m_nUserInput);
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
		case ScriptType::OnAskMenu:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0)
				pScript->Abort();
			else if (nAction == 1)
				pScript->GetConverstaionState().m_nUserInput = iPacket->Decode4();
			lua_pushinteger(L, pScript->GetConverstaionState().m_nUserInput);
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
		case ScriptType::OnAskAvatar:
		{
			iPacket->Decode1();
			iPacket->Decode1();
			nAction = iPacket->Decode1();
			if (nAction == 0)
				pScript->Abort();
			else if (nAction == 1)
			{
				auto& refInfo = pScript->GetLastConversationInfo();
				pScript->GetConverstaionState().m_nUserInput = iPacket->Decode1();
				if (pScript->GetConverstaionState().m_nUserInput < refInfo.m_aIntObj.size())
				{
					pScript->GetUser()->SendCharacterStat(false, QWUser::SetHair(pScript->GetUser(), refInfo.m_aIntObj[pScript->GetConverstaionState().m_nUserInput]));
					pScript->GetUser()->OnAvatarModified();
				}	
				refInfo.m_aIntObj.clear();
			}
			lua_pushinteger(L, pScript->GetConverstaionState().m_nUserInput);
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
	}
	pScript->GetConverstaionState().m_bPaging = false;
	if (pScript->IsDone())
	{
		FreeObj( pScript );
		return;
	}
	pScript->Notify();
}

int ScriptNPC::SelfSay(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptType::OnSay;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	CheckMessageParameter(L, 3, &info);
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskAvatar(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);
	int nTicket = (int)luaL_checkinteger(L, 3); //ticket
	//int nArgs = lua_gettop(L); 

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptType::OnAskAvatar;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	self->RetrieveArray(info.m_aIntObj, -1);
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskText(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);

	const char* text = luaL_checkstring(L, 2);
	const char * defaultText = luaL_checkstring(L, 3);
	int nMinValue = (int)luaL_checkinteger(L, 4);
	int nMaxValue = (int)luaL_checkinteger(L, 5);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptType::OnAskText;
	info.m_sTalkText = text;
	info.m_aStrObj.push_back(defaultText);
	info.m_nSpeakerTemplateID = self->GetID();
	info.m_aIntObj.push_back(nMinValue);
	info.m_aIntObj.push_back(nMaxValue);
	//====================

	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskNumber(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);
	int nDefaultValue = (int)luaL_checkinteger(L, 3);
	int nMinValue = (int)luaL_checkinteger(L, 4);
	int nMaxValue = (int)luaL_checkinteger(L, 5);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptType::OnAskNumber;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	info.m_aIntObj.push_back(nDefaultValue);
	info.m_aIntObj.push_back(nMinValue);
	info.m_aIntObj.push_back(nMaxValue);
	//====================

	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskYesNo(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptType::OnAskYesNo;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	CheckMessageParameter(L, 3, &info);
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskAcceptDecline(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptType::OnAskAcceptDecline;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	CheckMessageParameter(L, 3, &info);
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskMenu(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);
	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptType::OnAskMenu;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	CheckMessageParameter(L, 3, &info);
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::Debug(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);
	WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[Script Debug]%s\n", text);
	return 1;
}

void ScriptNPC::CheckMessageParameter(lua_State * L, int nStartIndex, void * pInfo_)
{
	auto pInfo = (Script::NPCConverstaionInfo*)pInfo_;
	int nArgs = lua_gettop(L), nValue;
	for (int i = nStartIndex; i <= nArgs; ++i)
	{
		nValue = (int)lua_tointeger(L, i);
		pInfo->m_nParam |= nValue;
		pInfo->m_nSpeakerTypeID = 3;
		pInfo->m_nSpeakerTemplateID_ = pInfo->m_nSpeakerTemplateID;
	}
}

void ScriptNPC::MakeMessagePacket(lua_State * L, void * pInfo_)
{
	Script *self = (Script*)L->selfPtr;
	auto pInfo = (Script::NPCConverstaionInfo*)pInfo_;
	OutPacket oPacket;
	oPacket.Encode2((short)NPCPacketFlags::NPC_ScriptMessage);
	oPacket.Encode1(pInfo->m_nSpeakerTypeID);
	oPacket.Encode4(pInfo->m_nSpeakerTemplateID);
	oPacket.Encode1(0); //Unk, set another speaker template id
	oPacket.Encode1(pInfo->m_nMsgType);
	oPacket.Encode2(pInfo->m_nParam);
	oPacket.Encode1(pInfo->m_eColor);
	switch (pInfo->m_nMsgType)
	{
		case ScriptType::OnSay:
		{
			if (pInfo->m_nParam & 4)
				oPacket.Encode4(pInfo->m_nSpeakerTemplateID_);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			oPacket.Encode1(pInfo->m_nPage != 0 ? 1 : 0);
			oPacket.Encode1(self->GetConverstaionState().m_bPaging);
			oPacket.Encode4(pInfo->m_tWait);
			break;
		}
		case ScriptType::OnSayImage:
		{
			oPacket.Encode1((unsigned char)pInfo->m_aStrObj.size());
			for (const auto& str : pInfo->m_aStrObj)
				oPacket.EncodeStr(str);
			break;
		}
		case ScriptType::OnAskMenu:
		case ScriptType::OnAskYesNo:
		case ScriptType::OnAskAndroid:
		case ScriptType::OnAskPet:
		case ScriptType::OnAskPetAll:
		case ScriptType::OnAskActionPetEvolution:
		case ScriptType::OnAskAcceptDecline:
		{
			if (pInfo->m_nParam & 4)
				oPacket.Encode4(pInfo->m_nSpeakerTemplateID_);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			break;
		}
		case ScriptType::OnAskText:
		case ScriptType::OnAskNumber:
		{
			if (pInfo->m_nMsgType == ScriptType::OnAskText && (pInfo->m_nParam & 4))
				oPacket.Encode4(pInfo->m_nSpeakerTemplateID_);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			if (pInfo->m_nMsgType == ScriptType::OnAskText)
				oPacket.EncodeStr(pInfo->m_aStrObj[0]);
			else
				oPacket.Encode4(pInfo->m_aIntObj[0]);
			oPacket.Encode4(pInfo->m_aIntObj[0 + pInfo->m_nMsgType == ScriptType::OnAskNumber]);
			oPacket.Encode4(pInfo->m_aIntObj[1 + pInfo->m_nMsgType == ScriptType::OnAskNumber]);
			break;
		}
		case ScriptType::OnInitialQuiz:
		{
			oPacket.Encode1(0);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			oPacket.EncodeStr(pInfo->m_aStrObj[0]);
			oPacket.EncodeStr(pInfo->m_aStrObj[1]);
			oPacket.Encode4(pInfo->m_aIntObj[0]);
			oPacket.Encode4(pInfo->m_aIntObj[1]);
			oPacket.Encode4(pInfo->m_tWait);
			break;
		}
		case ScriptType::OnInitialSpeedQuiz:
		case ScriptType::OnICQuiz:
		case ScriptType::OnAskAvatar:
		{
			oPacket.Encode1(0); //bAngelicBuster
			oPacket.Encode1(0); //bZeroBeta
			oPacket.EncodeStr(pInfo->m_sTalkText);
			oPacket.Encode1((unsigned char)pInfo->m_aIntObj.size());
			for (const auto& value : pInfo->m_aIntObj)
				oPacket.Encode4(value);
			break;
		}
		case ScriptType::OnAskBoxText: 
		case ScriptType::OnAskSlideMenu: 
		case ScriptType::OnAskSelectMenu:
		case ScriptType::OnAskAngelicBuster:
		case ScriptType::OnAskMixHairNew:
		//case ScriptType::OnAskMixHairNewExZero:
		case ScriptType::OnAskScreenShinningStarMsg:
			break;
		case ScriptType::OnSayIllustration:
		case ScriptType::OnAskYesNoIllustration:
		case ScriptType::OnAskAvatarZero:
		case ScriptType::OnAskWeaponBox: 
		{
			oPacket.Encode2(0);
			oPacket.EncodeStr("");
			oPacket.EncodeStr("");
			oPacket.Encode2(0);
			oPacket.Encode2(0);
			oPacket.Encode2(0);
			oPacket.Encode2(0);
			break;
		}
		case ScriptType::OnAskUserSurvey:
		{
			oPacket.Encode4(0);
			oPacket.Encode1(1);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			break;
		}
		case ScriptType::OnAskMixHair:
		case ScriptType::OnAskMixHairExZero:
		case ScriptType::OnAskCustomMixHairAndProb:
		{
			oPacket.Encode1(0);
			oPacket.Encode4(0);
			oPacket.Encode4(0);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			break;
		}
		case  ScriptType::OnAskNumberUseKeyPad:
		case  ScriptType::OnSpinOffGuitarRhythmGame:
		case  ScriptType::OnGhostParkEnter:
		{
			oPacket.Encode4(0);
			break;
		}
	}
	if (!self->IsDone()) 
	{
		self->GetUser()->SendPacket(&oPacket);
		self->SetLastConversationInfo(*pInfo);
	}
}

int ScriptNPC::SelfSayNext(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);
	
	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptType::OnSay;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	info.m_nPage = self->GetConverstaionState().m_nCurPage;
	//====================

	self->GetConverstaionState().m_aPageStack.push_back(std::move(info));
	self->GetConverstaionState().m_bPaging = true;
	SelfSayNextImpl(L, self->GetConverstaionState().m_nCurPage++);
	self->Wait();
	return 1;
}

void ScriptNPC::SelfSayNextImpl(lua_State * L, int nPage)
{
	Script *self = (Script*)L->selfPtr;
	auto& refStack = self->GetConverstaionState().m_aPageStack;
	if (nPage >= refStack.size())
		return;
	auto& refInfo = refStack[nPage];

	CheckMessageParameter(L, 3, &refInfo);
	MakeMessagePacket(L, &refInfo);
}
