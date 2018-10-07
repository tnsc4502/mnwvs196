#include "ScriptNPCConversation.h"
#include "Script.h"
#include "User.h"
#include "QWUser.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\WvsLib\Script\lvm.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Logger\WvsLogger.h"

ScriptNPCConversation::ScriptNPCConversation()
{
}


ScriptNPCConversation::~ScriptNPCConversation()
{
}

void ScriptNPCConversation::Register(lua_State * L)
{

	luaL_Reg SelfMetatable[] = {
		{ NULL, NULL }
	};

	luaL_Reg SelfTable[] = {
		{ "askAvatar", SelfAskAvatar },
		{ "askText", SelfAskText },
		{ "askYesNo", SelfAskYesNo },
		{ "askNumber", SelfAskNumber },
		{ "askMenu", SelfAskMenu },
		{ "sayNext", SelfSayNext },
		{ "sayNextGroup", SelfSayNextGroup },
		{ "say", SelfSay },
		{ "pushArray", SelfPushArray },
		{ NULL, NULL }
	};
	luaW_register<Script>(L, "self", SelfTable, SelfMetatable, &(Script::GetSelf));
}

int ScriptNPCConversation::SelfSayNextGroup(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr); //+0
	int nPages = lua_gettop(L); //+1

	//note that the index should start from 2
	for (int i = 1; i <= nPages; )
	{
		auto msg = lua_tostring(L, i);
		WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "SelfSayNextGroup Args %d = %s\n", i, msg);
		OutPacket oPacket;
		oPacket.Encode2(0x56E); //opcode
		oPacket.Encode1(4);
		oPacket.Encode4(self->GetID());
		oPacket.Encode1(0);
		oPacket.Encode1((char)Script::Script::ScriptType::OnSay); //OnSay
		oPacket.Encode2(0);
		oPacket.Encode1(0);
		oPacket.EncodeStr(msg); //message
		oPacket.Encode1(i > 2 ? 1 : 0); //show next page
		oPacket.Encode1(i < nPages ? 1 : 0); //show prev page
		oPacket.Encode4(0);
		self->GetUser()->SendPacket(&oPacket);

		self->Wait();
		i += (self->GetUserIntInput() == 0 ? -1 : 1);
		if (i < 2)
			break;
	}

	lua_pushstring(L, "");
	return 1;
}

int ScriptNPCConversation::SelfSay(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 1);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->GetID());
	oPacket.Encode1(0);
	oPacket.Encode1((char)Script::ScriptType::OnSay); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode4(0);
	self->GetUser()->SendPacket(&oPacket);
	self->Wait();
	lua_pushstring(L, "");
	return 1;
}

int ScriptNPCConversation::SelfAskAvatar(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 1);
	int nTicket = (int)luaL_checkinteger(L, 2); //ticket
	int nArgs = lua_gettop(L);
	self->GetArrayObj().clear();
	for (int i = 3; i < nArgs; ++i)
		self->GetArrayObj().push_back((int)lua_tointeger(L, i));
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->GetID());
	oPacket.Encode1(0);
	oPacket.Encode1((char)Script::ScriptType::OnAskAvatar); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0); //bAngelicBuster
	oPacket.Encode1(0); //bZeroBeta
	oPacket.EncodeStr(text);
	oPacket.Encode1((char)self->GetArrayObj().size());
	for (auto& v : self->GetArrayObj())
		oPacket.Encode4(v);
	self->GetUser()->SendPacket(&oPacket);
	self->Wait();
	return 1;
}

int ScriptNPCConversation::SelfAskText(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 1);
	const char * defaultText = luaL_checkstring(L, 2);
	int nMinValue = (int)luaL_checkinteger(L, 3);
	int nMaxValue = (int)luaL_checkinteger(L, 4);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->GetID());
	oPacket.Encode1(0);
	oPacket.Encode1((char)Script::ScriptType::OnAskText); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	oPacket.EncodeStr(defaultText);
	oPacket.Encode2(nMinValue);
	oPacket.Encode2(nMaxValue);
	self->GetUser()->SendPacket(&oPacket);
	self->Wait();
	return 1;
}

int ScriptNPCConversation::SelfAskNumber(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 1);
	int nDefaultValue = (int)luaL_checkinteger(L, 2);
	int nMinValue = (int)luaL_checkinteger(L, 3);
	int nMaxValue = (int)luaL_checkinteger(L, 4);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->GetID());
	oPacket.Encode1(0);
	oPacket.Encode1((char)Script::ScriptType::OnAskNumber); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	oPacket.Encode4(nDefaultValue);
	oPacket.Encode4(nMinValue);
	oPacket.Encode4(nMaxValue);
	self->GetUser()->SendPacket(&oPacket);
	self->Wait();
	return 1;
}

int ScriptNPCConversation::SelfAskYesNo(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 1);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->GetID());
	oPacket.Encode1(0);
	oPacket.Encode1((char)Script::ScriptType::OnAskYesNo); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	self->GetUser()->SendPacket(&oPacket);
	self->Wait();

	//
	return 1;
}

int ScriptNPCConversation::SelfAskMenu(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 1);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->GetID());
	oPacket.Encode1(0);
	oPacket.Encode1((char)Script::ScriptType::OnAskMenu); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	self->GetUser()->SendPacket(&oPacket);
	self->Wait();
	return 1;
}

int ScriptNPCConversation::SelfPushArray(lua_State * L)
{
	Script* self = (Script*)L->selfPtr;
	int nValue = (int)luaL_checkinteger(L, 1);
	self->GetArrayObj().push_back(nValue);
	return 1;
}

int ScriptNPCConversation::SelfSayNext(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 1);
	int nCurPage = 1;
	int nNextPage = 1;
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->GetID());
	oPacket.Encode1(0);
	oPacket.Encode1((char)Script::ScriptType::OnSay); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	oPacket.Encode1(nCurPage > 0 ? 1 : 0);
	oPacket.Encode1(nCurPage < nNextPage ? 1 : 0);
	oPacket.Encode4(0);
	self->GetUser()->SendPacket(&oPacket);

	self->Wait();
	lua_pushinteger(L, nCurPage + (self->GetUserIntInput() == 0 ? -1 : 1));
	return 1;
}