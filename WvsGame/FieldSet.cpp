#include "FieldSet.h"
#include "Field.h"
#include "FieldMan.h"
#include "User.h"
#include "ScriptMan.h"
#include <functional>
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"

FieldSet::FieldSet()
{
}


FieldSet::~FieldSet()
{
	m_pFieldSetTimer->Abort();
	delete m_pFieldSetTimer;
	if (m_pScript) 
	{
		m_pScript->Abort();
		delete m_pScript;
	}
}

void FieldSet::Init(const std::string & sCfgFilePath)
{
	auto pCfg = ConfigLoader::Get(sCfgFilePath);
	m_aFieldID = pCfg->GetArray<int>("FieldList");
	Field *pField;
	for (auto& nFieldID : m_aFieldID) 
	{
		pField = FieldMan::GetInstance()->GetField(nFieldID);
		pField->SetFieldSet(this);
		m_aField.push_back(pField);
	}
	m_sFieldSetName = pCfg->StrValue("FieldSetName");
	m_sScriptName = pCfg->StrValue("ScriptName");
	m_nTimeLimit = pCfg->IntValue("TimeLimit");

	m_pScript = ScriptMan::GetInstance()->GetScript(m_sScriptName, 0);
	lua_pcall(m_pScript->GetLuaState(), 0, 0, 0); //Initialize all functions
	auto bindT = std::bind(&FieldSet::Update, this);
	m_pFieldSetTimer = AsyncScheduler::CreateTask(bindT, 1000, true);
}

const std::string & FieldSet::GetFieldSetName() const
{
	return m_sFieldSetName;
}

void FieldSet::Enter(int nCharacterID, int nFieldInfo)
{
	auto pUser = User::FindUser(nCharacterID);
	if (pUser)
	{
		TryEnter({ pUser }, nFieldInfo, nCharacterID);
	}
}

int FieldSet::TryEnter(const std::vector<User*>& lpUser, int nFieldIdx, int nEnterChar)
{
	for (auto& pUser : lpUser)
	{
		pUser->TryTransferField(
			m_aFieldID[nFieldIdx],
			""
		);
	}
	lua_getglobal(m_pScript->GetLuaState(), "onUserEnter");
	lua_resume(m_pScript->GetLuaState(), m_pScript->GetLuaCoroutineState(), 0);
	//auto pUpdateTimer = Async
	m_pFieldSetTimer->Start();
	m_nStartTime = GameDateTime::GetTime();
	return 0;
}

void FieldSet::Update()
{
	auto tCur = GameDateTime::GetTime();
	if (tCur - m_nStartTime > m_nTimeLimit * 1000)
	{
		lua_getglobal(m_pScript->GetLuaState(), "onTimeout");
		auto nResult = lua_resume(m_pScript->GetLuaState(), m_pScript->GetLuaCoroutineState(), 0);
		m_pFieldSetTimer->Pause();
	}
}

void FieldSet::OnUserEnterField(User* pUser)
{
	OutPacket oPacket;
	if (m_pFieldSetTimer->IsStarted()) 
	{
		MakeClockPacket(oPacket);
		pUser->SendPacket(&oPacket);
	}
}

void FieldSet::MakeClockPacket(OutPacket & oPacket)
{
	auto tCur = GameDateTime::GetTime();
	oPacket.Encode2((short)FieldSendPacketFlag::Field_OnClock);
	oPacket.Encode1(2);
	oPacket.Encode4(((m_nStartTime + m_nTimeLimit * 1000) - GameDateTime::GetTime()) / 1000);
}
