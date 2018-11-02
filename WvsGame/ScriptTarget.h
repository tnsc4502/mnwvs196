#pragma once
#include "..\WvsLib\Script\lua.h"

class User;

class ScriptTarget
{
	User *m_pUser;

public:
	ScriptTarget();
	~ScriptTarget();

	User* GetUser();
	static ScriptTarget* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptTarget* p);
	static void Register(lua_State* L);
	static int TargetChatMessage(lua_State* L);
	static int TargetNoticeMessage(lua_State* L);
	static int TargetInventory(lua_State* L);
};

