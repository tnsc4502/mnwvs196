#include "ScriptMan.h"
#include "ScriptNPCConversation.h"

ScriptMan * ScriptMan::GetInstance()
{
	static ScriptMan* pInstance = new ScriptMan;
	return pInstance;
}

Script * ScriptMan::GetScript(const std::string & file, int nNpcID)
{
	auto pScript = new Script(file, nNpcID, {
		&ScriptNPCConversation::Register
	});
	luaL_openlibs(pScript->L);
	return pScript;
}
