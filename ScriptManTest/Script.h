#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>

// Lua is written in C, so compiler needs to know how to link its libraries
extern "C"
{
#include "..\ScriptLib\lapi.h"
#include "..\ScriptLib\lua.h"
#include "..\ScriptLib\lauxlib.h"
#include "..\ScriptLib\lualib.h"
}

#include "..\ScriptLib\luawrapper.hpp"
class Script
{
	std::mutex m_mtxWaitLock;
	std::condition_variable m_cndVariable;


	static int Self_askText(lua_State* L);
	static Script* Self_new(lua_State* L);

	static luaL_Reg Self_table[];
	static luaL_Reg Self_metatable[];

	friend class ScriptMan;
	std::string fileName;

public:
	int id;

	lua_State* L = luaL_newstate();

	void Wait()
	{
		std::unique_lock<std::mutex> lock(m_mtxWaitLock);
		printf("Ready to wait\n");
		m_cndVariable.wait(lock);
		printf("Finish wait\n");
	}

	void Notify() 
	{ 
		m_cndVariable.notify_one(); 
	}

	Script(const std::string& file);

	static int luaopen_Self(lua_State* L);

	void Run();

	~Script();
};