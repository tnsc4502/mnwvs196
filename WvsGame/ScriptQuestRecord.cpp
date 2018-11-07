#include "ScriptQuestRecord.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "User.h"
#include "QWUQuestRecord.h"
#include "Script.h"


ScriptQuestRecord::ScriptQuestRecord()
{
}


ScriptQuestRecord::~ScriptQuestRecord()
{
}

void ScriptQuestRecord::SetUser(User * pUser)
{
	m_pUser = pUser;
}

ScriptQuestRecord * ScriptQuestRecord::GetSelf(lua_State * L)
{
	int nUserID = (int)luaL_checkinteger(L, 1);
	auto pUser = User::FindUser(nUserID);
	if (!pUser)
		return nullptr;
	ScriptQuestRecord *p = AllocObj(ScriptQuestRecord);
	p->m_pUser = pUser;
	return p;
}

void ScriptQuestRecord::DestroySelf(lua_State * L, ScriptQuestRecord * p)
{
	FreeObj(p);
}

void ScriptQuestRecord::Register(lua_State * L)
{
	luaL_Reg QRMetatable[] = {
		{ "getState", QuestRecordGetState },
		{ "setState", QuestRecordSetState },
		{ NULL, NULL }
	};

	luaL_Reg QRTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptQuestRecord>(L, "QuestRecord",
		QRTable,
		QRMetatable,
		&(ScriptQuestRecord::GetSelf),
		&(ScriptQuestRecord::DestroySelf));
}

int ScriptQuestRecord::QuestRecordGetState(lua_State * L)
{
	ScriptQuestRecord* self = luaW_check<ScriptQuestRecord>(L, 1);
	int nQuestID = (int)luaL_checkinteger(L, 2);
	int nResult = QWUQuestRecord::GetState(self->m_pUser, nQuestID);
	lua_pushinteger(L, nResult);
	return 1;
}

int ScriptQuestRecord::QuestRecordSetState(lua_State * L)
{
	ScriptQuestRecord* self = luaW_check<ScriptQuestRecord>(L, 1);
	int nQuestID = (int)luaL_checkinteger(L, 2);
	int nState = (int)luaL_checkinteger(L, 3);
	if (nState == 0)
		QWUQuestRecord::Remove(self->m_pUser, nQuestID, false);
	else if (nState == 1)
		QWUQuestRecord::Set(
			self->m_pUser,
			nQuestID,
			lua_gettop(L) > 3 ? luaL_checkstring(L, 4) : "");
	else if (nState == 2)
		QWUQuestRecord::SetComplete(self->m_pUser, nQuestID);
	return 1;
}
