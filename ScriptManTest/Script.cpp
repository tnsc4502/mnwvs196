#include "Script.h"

luaL_Reg Script::Self_metatable[] = { 
	{ "askText", Self_askText },
	{ NULL, NULL }
};

luaL_Reg Script::Self_table[] = {
	{ NULL, NULL }
};

Script * Script::Self_new(lua_State * L) 
{
	long long ptr = luaL_checkinteger(L, 1);
	return (Script*)(ptr);
}

Script::Script(const std::string & file)
{
	fileName = file;
}

int Script::luaopen_Self(lua_State * L)
{
	luaW_register<Script>(L, "Self", Self_table, Self_metatable, Self_new);
	return 1;
}

int Script::Self_askText(lua_State * L)
{
	Script* self = luaW_check<Script>(L, 1);
	const char* text = luaL_checkstring(L, 2);

	self->Wait();
	lua_pushstring(L, "");
	return 1;
}

void Script::Run()
{
	lua_pushinteger(L, (long long)(this));
	lua_setglobal(L, "selfID");
	luaopen_Self(L);
	if (luaL_loadfile(L, fileName.c_str()))
		std::cout << "Error, can't open script" << std::endl;
	lua_pcall(L, 0, 0, 0);
}
Script::~Script()
{
	if (L) lua_close(L);
}
