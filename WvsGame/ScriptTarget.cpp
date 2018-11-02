#include "ScriptTarget.h"
#include "ScriptInventory.h"
#include "Script.h"
#include "User.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

ScriptTarget::ScriptTarget()
{
}

ScriptTarget::~ScriptTarget()
{
}

User * ScriptTarget::GetUser()
{
	return m_pUser;
}

ScriptTarget * ScriptTarget::GetSelf(lua_State * L)
{
	int nUserID = (int)luaL_checkinteger(L, 1);
	auto pUser = User::FindUser(nUserID);
	if (pUser == nullptr)
		return nullptr;
	ScriptTarget* p = AllocObj(ScriptTarget);
	p->m_pUser = pUser;
	return p;
}

void ScriptTarget::DestroySelf(lua_State * L, ScriptTarget * p)
{
	FreeObj(p);
}

void ScriptTarget::Register(lua_State * L)
{
	luaL_Reg TargetMetatable[] = {
		{ "noticeMsg", TargetNoticeMessage },
		{ "chatMsg", TargetChatMessage },
		{ "inventory", TargetInventory },
		{ NULL, NULL }
	};

	luaL_Reg TargetTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptTarget>(L, "User", 
		TargetTable, 
		TargetMetatable, 
		&(ScriptTarget::GetSelf),
		&(ScriptTarget::DestroySelf));
}

int ScriptTarget::TargetChatMessage(lua_State * L)
{
	ScriptTarget* self = luaW_check<ScriptTarget>(L, 1);
	int nType = (int)luaL_checkinteger(L, 2);
	const char* sMsg = luaL_checkstring(L, 3);
	self->m_pUser->SendChatMessage(nType, sMsg);
	lua_pushinteger(L, 1);
	return 1;
}

int ScriptTarget::TargetNoticeMessage(lua_State * L)
{
	ScriptTarget* self = luaW_check<ScriptTarget>(L, 1);
	int nType = (int)luaL_checkinteger(L, 2);
	const char* sMsg = luaL_checkstring(L, 3);
	self->m_pUser->SendNoticeMessage(nType, sMsg);
	lua_pushinteger(L, 1);
	return 1;
}

int ScriptTarget::TargetInventory(lua_State * L)
{
	ScriptTarget* self = luaW_check<ScriptTarget>(L, 1);
	int numargs = lua_gettop(L);
	ScriptInventory* obj = (self->m_pUser == nullptr ? nullptr : AllocObj(ScriptInventory));
	if (obj)
		obj->SetUser(self->m_pUser);

	luaW_push<ScriptInventory>(L, obj); // ... args... ud
	luaW_hold<ScriptInventory>(L, obj);
	lua_insert(L, -1 - numargs); // ... ud args...
	luaW_postconstructor<ScriptInventory>(L, numargs); // ... ud
	return 1;
}
