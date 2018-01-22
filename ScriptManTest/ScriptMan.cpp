#include "ScriptMan.h"

Character * ScriptMan::Character_new(lua_State * L) {
	const char* name = luaL_checkstring(L, 1);
	int hp = luaL_checknumber(L, 2);
	return new Character(name, hp);
}

int ScriptMan::Character_getName(lua_State * L) {
	Character* character = luaW_check<Character>(L, 1);
	lua_pushstring(L, character->getName());
	return 1;
}

int ScriptMan::Character_askNumber(lua_State * L) {
	Character* character = luaW_check<Character>(L, 1);
	const char* text = luaL_checkstring(L, 2);
	printf("askNumber = %s\n", text);
	lua_pushstring(L, character->getName());
	return 1;
}

int ScriptMan::Character_askText(lua_State * L) {
	Character* character = luaW_check<Character>(L, 1);
	const char* text = luaL_checkstring(L, 2);
	printf("Character_askText = %s\n", text);
	lua_pushstring(L, "");
	return 1;
}

int ScriptMan::Character_getHealth(lua_State * L) {
	Character* character = luaW_check<Character>(L, 1);
	lua_pushnumber(L, character->getHealth());
	return 1;
}

int ScriptMan::Character_setHealth(lua_State * L) {
	Character* character = luaW_check<Character>(L, 1);
	int hp = luaL_checknumber(L, 2);
	character->setHealth(hp);
	return 0;
}

ScriptMan * ScriptMan::GetInstance()
{
	static ScriptMan* pInstance = new ScriptMan;
	return pInstance;
}

int ScriptMan::luaopen_Character(lua_State * L) {
	luaW_register<Character>(L, "Character", Character_table, Character_metatable, Character_new);
	return 1;
}

Script * ScriptMan::GetScript(const std::string & file)
{
	auto pScript = new Script(file);
	luaL_openlibs(pScript->L);
	luaopen_Character(pScript->L);
	//luaW_push(pScript->L, pScript);
	return pScript;
}

luaL_Reg ScriptMan::Character_metatable[] = {
	{ "getName", Character_getName },
	{ "getHealth", Character_getHealth },
	{ "setHealth", Character_setHealth },
	{ "askNumber", Character_askNumber },
	{ "askText", Character_askText },
	{ NULL, NULL }
};

luaL_Reg ScriptMan::Character_table[] = {
	{ NULL, NULL }
};