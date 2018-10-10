#include "ScriptMan.h"
#include "ScriptNPCConversation.h"
#include "ScriptInventory.h"
#include "ScriptFieldSet.h"

ScriptMan * ScriptMan::GetInstance()
{
	static ScriptMan* pInstance = new ScriptMan;
	return pInstance;
}

Script * ScriptMan::GetScript(const std::string & file, int nNpcID)
{
	auto pScript = new Script(file, nNpcID, {
		&ScriptNPCConversation::Register,
		&ScriptInventory::Register,
		&ScriptFieldSet::Register
	});
	pScript->m_pOnPacketInvoker = &(ScriptNPCConversation::OnPacket);
	luaL_openlibs(pScript->L);
	return pScript;
}
