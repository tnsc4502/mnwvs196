#pragma once
#include "..\WvsLib\Script\lua.h"

class Script;

class ScriptFieldSet
{
public:
	ScriptFieldSet();
	~ScriptFieldSet();
	static void Register(lua_State* L);

	static int FieldSetEnter(lua_State* L);
};

