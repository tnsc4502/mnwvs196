#include "ScriptMan.h"
#include "ScriptNPCConversation.h"
#include "ScriptInventory.h"
#include "ScriptFieldSet.h"
#include "ScriptTarget.h"
#include "ScriptPacket.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

ScriptMan * ScriptMan::GetInstance()
{
	static ScriptMan* pInstance = new ScriptMan;
	return pInstance;
}

Script * ScriptMan::GetScript(const std::string & file, int nNpcID)
{
	auto pScript = AllocObjCtor(Script)(
		file, 
		nNpcID, 
		std::vector<void(*)(lua_State*)>({
		&ScriptNPCConversation::Register,
		&ScriptInventory::Register,
		&ScriptFieldSet::Register,
		&ScriptTarget::Register,
		&ScriptPacket::Register
	}));
	if (pScript && pScript->Init())
	{
		pScript->m_pOnPacketInvoker = &(ScriptNPCConversation::OnPacket);
		luaL_openlibs(pScript->L);
		return pScript;
	}
	else if (pScript)
		FreeObj(pScript);
	return nullptr;
}
