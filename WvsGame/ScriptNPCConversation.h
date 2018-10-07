#pragma once
#include "..\WvsLib\Script\lua.h"

class ScriptNPCConversation
{
public:
	ScriptNPCConversation();
	~ScriptNPCConversation();

	static void Register(lua_State* L);
	static int SelfSayNextGroup(lua_State* L);
	static int SelfSay(lua_State* L);
	static int SelfAskAvatar(lua_State* L);
	static int SelfSayNext(lua_State* L);
	static int SelfAskText(lua_State* L);
	static int SelfAskNumber(lua_State* L);
	static int SelfAskYesNo(lua_State* L);
	static int SelfAskMenu(lua_State* L);
	static int SelfPushArray(lua_State* L);
};

