#include "ScriptVM.h"
#include <functional>
#include "..\Common\Utility\Task\AsnycScheduler.h"

ScriptVM::ScriptVM()
{
	auto funcBind = std::bind(&ScriptVM::ScriptGC, this);
	auto asyncGC = AsnycScheduler::CreateTask(funcBind, 5000, true);
	asyncGC->Start();
}


ScriptVM::~ScriptVM()
{
}

void ScriptVM::ScriptGC()
{
	std::lock_guard<std::mutex> lock(m_mtxRegTableLock);
	std::vector<Script*> aRemoved;
	for (auto& iter = m_aSciprtSet.begin(); iter != m_aSciprtSet.end(); ++iter)
	{
		//if ((*iter)->IsDone())
		{
			aRemoved.push_back(*iter);
			//ClearSript((*iter));
		}
	}
	for (auto& pScript : aRemoved) {
		m_aSciprtSet.erase(pScript);
		ClearScript(pScript);
	}
}

ScriptVM * ScriptVM::GetInstance()
{
	static ScriptVM* pInstance = new ScriptVM;
	return pInstance;
}

void ScriptVM::ClearScript(Script * pScript)
{
	//printf("Clear Script File\n");
	//pScript->GetThread()->~thread();
	//delete pScript;
	//printf("Clear Script File 2\n");
}

void ScriptVM::RegisterScript(Script * pScript)
{
	std::lock_guard<std::mutex> lock(m_mtxRegTableLock);
	m_aSciprtSet.insert(pScript);
}
