#pragma once
#include "..\WvsLib\Script\lua.h"

class User;

class ScriptQuestRecord
{
	User *m_pUser;
public:
	ScriptQuestRecord();
	~ScriptQuestRecord();

	void SetUser(User *pUser);

	static ScriptQuestRecord* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptQuestRecord* p);
	static void Register(lua_State* L);
	static int QuestRecordGetState(lua_State* L);
	static int QuestRecordSetState(lua_State* L);
};

