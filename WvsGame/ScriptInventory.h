#pragma once
#include "..\WvsLib\Script\lua.h"

class Script;

class ScriptInventory
{
public:
	ScriptInventory();
	~ScriptInventory();

	static void Register(lua_State* L);

	static int InventoryExchange(lua_State* L);
};

