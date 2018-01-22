#pragma once

#include <string>
#include <vector>
#include <iostream>


// Lua is written in C, so compiler needs to know how to link its libraries
extern "C"
{
#include "..\ScriptLib\lapi.h"
#include "..\ScriptLib\lua.h"
#include "..\ScriptLib\lauxlib.h"
#include "..\ScriptLib\lualib.h"
}

#include "..\ScriptLib\luawrapper.hpp"

#include "Script.h"
#include "Character.h"

class ScriptMan
{
public:

	static luaL_Reg Character_table[];

	static luaL_Reg Character_metatable[];

	static Character* Character_new(lua_State* L);

	static int Character_getName(lua_State* L);

	static int Character_askNumber(lua_State* L);

	static int Character_askText(lua_State* L);

	static int Character_getHealth(lua_State* L);

	static int Character_setHealth(lua_State* L);

	static ScriptMan* GetInstance();

	static int luaopen_Character(lua_State* L);

	Script* GetScript(const std::string& file);
};
