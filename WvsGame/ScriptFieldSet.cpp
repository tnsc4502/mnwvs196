#include "ScriptFieldSet.h"
#include "Script.h"
#include "FieldMan.h"
#include "FieldSet.h"
#include "User.h"

ScriptFieldSet::ScriptFieldSet()
{
}


ScriptFieldSet::~ScriptFieldSet()
{
}

void ScriptFieldSet::Register(lua_State * L)
{
	luaL_Reg FieldSetMetatable[] = {
		{ NULL, NULL }
	};

	luaL_Reg FieldSetTable[] = {
		{ "enter", FieldSetEnter },
		{ NULL, NULL }
	};

	luaW_register<Script>(L, "fieldSet", FieldSetTable, FieldSetMetatable, &(Script::GetSelf));
}

int ScriptFieldSet::FieldSetEnter(lua_State * L)
{
	Script *self = (Script*)L->selfPtr;
	const char* sFieldSetName = luaL_checkstring(L, 1);
	auto pFieldSet = FieldMan::GetInstance()->GetFieldSet(sFieldSetName);
	if (pFieldSet == nullptr)
	{
		lua_pushinteger(L, 0);
		return 1;
	}
	pFieldSet->Enter(self->GetUser()->GetUserID(), 0);
	lua_pushinteger(L, 1);
	return 1;
}
