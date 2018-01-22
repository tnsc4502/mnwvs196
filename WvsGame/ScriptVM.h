#pragma once

#include <mutex>
#include <set>

#include "Script.h"
#include "ScriptMan.h"

class Script;

class ScriptVM
{
	std::mutex m_mtxRegTableLock;
	std::set<Script*> m_aSciprtSet;

	ScriptVM();
	~ScriptVM();

	void ScriptGC();
public:
	static ScriptVM* GetInstance();

	void ClearScript(Script *pScript);
	void RegisterScript(Script *pScript);
};

