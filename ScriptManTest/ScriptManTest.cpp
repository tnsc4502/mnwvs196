// ScriptManTest.cpp : Defines the entry point for the console application.
//

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


#include <thread>
#include <mutex>
#include <condition_variable>

#include "ScriptMan.h"
#include "Script.h"

#include <map>

void Run(Script* pScript)
{
	pScript->Run();
}

int main()
{

	std::map<int, Script*> mScripts;
	for (int i = 0; i < 100; ++i) {
		auto pScript = ScriptMan::GetInstance()->GetScript("test.lua");
		pScript->id = i;
		mScripts[i] = pScript;
		//pScript->Self_askText(pScript->L);
		std::thread t(Run, pScript);
		t.detach();
	//	delete pScript;
	}
	int x;
	//std::cin >> x;
	while (std::cin >> x)
	{
		auto pScript = mScripts[x];
		pScript->Notify();
	}
}