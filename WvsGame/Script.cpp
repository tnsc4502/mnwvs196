#include "Script.h"
#include <memory>
#include "User.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "QWUser.h"
#include "..\ScriptLib\lvm.h"
#include "..\Common\Net\InPacket.h"
#include "..\Common\Net\OutPacket.h"

#include "..\WvsLib\Logger\WvsLogger.h"

luaL_Reg Script::SelfMetatable[] = {
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

luaL_Reg Script::SelfTable[] = {
	{ NULL, NULL }
};

Script * Script::GetSelf(lua_State * L)
{
	long long ptr = luaL_checkinteger(L, 1);
	return (Script*)(ptr);
}

void Script::Wait()
{
	std::unique_lock<std::mutex> lock(m_mtxWaitLock);
	WvsLogger::LogRaw("Ready to wait\n");
	m_cndVariable.wait(lock);
	WvsLogger::LogRaw("Finish wait\n");
}

void Script::Notify()
{
	std::lock_guard<std::mutex> lock(m_mtxWaitLock);
	m_cndVariable.notify_one();
}

Script::Script(const std::string & file, int nNpcID) :
	L(luaL_newstate())
{
	m_fileName = file;
	m_nID = nNpcID;
	lua_pushinteger(L, (long long int)(this));
	lua_setglobal(L, "selfPtr");
	lua_pushinteger(L, (long long int)(m_pUser));
	lua_setglobal(L, "userPtr");
	LuaRegisterSelf(L);
	if (luaL_loadfile(L, m_fileName.c_str()))
		WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "Error, can't open script.\n");
}

int Script::LuaRegisterSelf(lua_State * L)
{
	luaW_register<Script>(L, "Self", SelfTable, SelfMetatable, GetSelf);
	return 1;
}

void Script::SetUser(User * pUser)
{
	m_pUser = pUser;
}

std::thread * Script::GetThread()
{
	return m_pThread;
}

void Script::SetThread(std::thread * pThread)
{
	m_pThread = pThread;
}

int Script::SelfSayNextGroup(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1); //+1
	int nPages = lua_gettop(L); //+1

	//note that the index should start from 2
	for (int i = 2; i <= nPages; )
	{
		auto msg = lua_tostring(L, i);
		WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "SelfSayNextGroup Args %d = %s\n", i, msg);
		OutPacket oPacket;
		oPacket.Encode2(0x56E); //opcode
		oPacket.Encode1(4);
		oPacket.Encode4(self->m_nID);
		oPacket.Encode1(0);
		oPacket.Encode1((char)ScriptType::OnSay); //OnSay
		oPacket.Encode2(0);
		oPacket.Encode1(0);
		oPacket.EncodeStr(msg); //message
		oPacket.Encode1(i > 2 ? 1 : 0); //show next page
		oPacket.Encode1(i < nPages ? 1 : 0); //show prev page
		oPacket.Encode4(0);
		self->m_pUser->SendPacket(&oPacket);

		self->Wait();
		i += (self->m_nUserInput == 0 ? -1 : 1);
		if (i < 2)
			break;
	}

	lua_pushstring(L, "");
	return 1;
}

int Script::SelfSay(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1);
	const char* text = luaL_checkstring(L, 2);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->m_nID);
	oPacket.Encode1(0);
	oPacket.Encode1((char)ScriptType::OnSay); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode4(0);
	self->m_pUser->SendPacket(&oPacket);
	self->Wait();
	lua_pushstring(L, "");
	return 1;
}

int Script::SelfAskAvatar(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1);
	const char* text = luaL_checkstring(L, 2);
	self->m_nUserInput = luaL_checkinteger(L, 3); //ticket
	int nArgs = lua_gettop(L);
	self->m_aArrayObj.clear();
	for (int i = 4; i < nArgs; ++i)
		self->m_aArrayObj.push_back(lua_tointeger(L, i));
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->m_nID);
	oPacket.Encode1(0);
	oPacket.Encode1((char)ScriptType::OnAskAvatar); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0); //bAngelicBuster
	oPacket.Encode1(0); //bZeroBeta
	oPacket.EncodeStr(text);
	oPacket.Encode1((char)self->m_aArrayObj.size());
	for (auto& v : self->m_aArrayObj)
		oPacket.Encode4(v);
	self->m_pUser->SendPacket(&oPacket);
	self->Wait();
	lua_pushinteger(L, self->m_nUserInput);
	return 1;
}

int Script::SelfAskText(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1);
	const char* text = luaL_checkstring(L, 2);
	const char * defaultText = luaL_checkstring(L, 3);
	int nMinValue = (int)luaL_checkinteger(L, 4);
	int nMaxValue = (int)luaL_checkinteger(L, 5);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->m_nID);
	oPacket.Encode1(0);
	oPacket.Encode1((char)ScriptType::OnAskText); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	oPacket.EncodeStr(defaultText);
	oPacket.Encode2(nMinValue);
	oPacket.Encode2(nMaxValue);
	self->m_pUser->SendPacket(&oPacket);
	self->Wait();
	lua_pushstring(L, self->m_strUserInput.c_str());
	return 1;
}

int Script::SelfAskNumber(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1);
	const char* text = luaL_checkstring(L, 2);
	int nDefaultValue = (int)luaL_checkinteger(L, 3);
	int nMinValue = (int)luaL_checkinteger(L, 4);
	int nMaxValue = (int)luaL_checkinteger(L, 5);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->m_nID);
	oPacket.Encode1(0);
	oPacket.Encode1((char)ScriptType::OnAskNumber); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	oPacket.Encode4(nDefaultValue);
	oPacket.Encode4(nMinValue);
	oPacket.Encode4(nMaxValue);
	self->m_pUser->SendPacket(&oPacket);
	self->Wait();
	lua_pushinteger(L, self->m_nUserInput);
	return 1;
}

int Script::SelfAskYesNo(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1);
	const char* text = luaL_checkstring(L, 2);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->m_nID);
	oPacket.Encode1(0);
	oPacket.Encode1((char)ScriptType::OnAskYesNo); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	self->m_pUser->SendPacket(&oPacket);
	self->Wait();
	lua_pushinteger(L, self->m_nUserInput);
	return 1;
}

int Script::SelfAskMenu(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1);
	const char* text = luaL_checkstring(L, 2);
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->m_nID);
	oPacket.Encode1(0);
	oPacket.Encode1((char)ScriptType::OnAskMenu); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	self->m_pUser->SendPacket(&oPacket);
	self->Wait();
	lua_pushinteger(L, self->m_nUserInput);
	return 1;
}

int Script::SelfPushArray(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1);
	int nValue = (int)luaL_checkinteger(L, 2);
	self->m_aArrayObj.push_back(nValue);
	return 1;
}

int Script::SelfSayNext(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1);
	const char* text = luaL_checkstring(L, 2);
	int nCurPage = 1;
	int nNextPage = 1;
	OutPacket oPacket;
	oPacket.Encode2(0x56E);
	oPacket.Encode1(4);
	oPacket.Encode4(self->m_nID);
	oPacket.Encode1(0);
	oPacket.Encode1((char)ScriptType::OnSay); //OnSay
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(text);
	oPacket.Encode1(nCurPage > 0 ? 1 : 0);
	oPacket.Encode1(nCurPage < nNextPage ? 1 : 0);
	oPacket.Encode4(0);
	self->m_pUser->SendPacket(&oPacket);

	self->Wait();
	lua_pushinteger(L, nCurPage + (self->m_nUserInput == 0 ? -1 : 1));
	return 1;
}

void Script::Run()
{
	lua_pcall(L, 0, 0, 0);
	std::lock_guard<std::mutex> lock(m_mtxWaitLock);
	m_pUser->SetScript(nullptr);
	if (L) lua_close(L);
	m_bDone = true;
}

void Script::Abort()
{
	try {
		luaL_error(L, "abort");
	}
	catch (...) {}
	std::lock_guard<std::mutex> lock(m_mtxWaitLock);
	m_pUser->SetScript(nullptr);
	if (L) lua_close(L);
	m_bDone = true;
}

bool Script::IsDone()
{
	std::lock_guard<std::mutex> lock(m_mtxWaitLock);
	return m_bDone;
}

void Script::OnPacket(InPacket * iPacket)
{
	char nMsgType = iPacket->Decode1(), nAction = 0;
	switch (nMsgType)
	{
		case ScriptType::OnSay:
		{
			nAction = iPacket->Decode1();
			if (nAction == (char)0xFF)
				Abort();
			else if (nAction == 0)
				m_nUserInput = 0;
			else if (nAction == 1)
				m_nUserInput = 1;
			break;
		}
		case ScriptType::OnAskYesNo:
		{
			nAction = iPacket->Decode1();
			if (nAction == (char)0xFF)
				Abort();
			else if (nAction == 0)
				m_nUserInput = 0;
			else if (nAction == 1)
				m_nUserInput = 1;
			break;
		}
		case ScriptType::OnAskText:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0)
				Abort();
			else if (nAction == 1)
				m_strUserInput = iPacket->DecodeStr();
			break;
		}
		case ScriptType::OnAskNumber:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0)
				Abort();
			else if (nAction == 1)
				m_nUserInput = iPacket->Decode4();
			break;
		}
		case ScriptType::OnAskMenu:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0)
				Abort();
			else if (nAction == 1)
				m_nUserInput = iPacket->Decode4();
			break;
		}
		case ScriptType::OnAskAvatar:
		{
			iPacket->Decode1();
			iPacket->Decode1();
			nAction = iPacket->Decode1();
			if (nAction == 0)
				Abort();
			else if (nAction == 1) 
			{
				m_pUser->SendCharacterStat(false, QWUser::SetHair(m_pUser, m_aArrayObj[iPacket->Decode1()]));
				m_aArrayObj.clear();
			}
			break;
		}
	}
}

Script::~Script()
{
}
