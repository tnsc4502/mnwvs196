#include "Script.h"
#include <memory>
#include "User.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "QWUser.h"
#include "..\WvsLib\Script\lvm.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Logger\WvsLogger.h"

Script * Script::GetSelf(lua_State * L)
{
	long long ptr = luaL_checkinteger(L, 1);
	return (Script*)(ptr);
}

void Script::Wait()
{
	lua_yield(L, 0);
}

void Script::Notify()
{
	Run();
}

Script::Script(const std::string & file, int nNpcID, const std::vector<void(*)(lua_State*)>& aReg) :
	L(luaL_newstate())
{
	C = lua_newthread(L);
	m_fileName = file;
	m_nID = nNpcID;
	for (auto& f : aReg)
		f(L);
	if (luaL_loadfile(L, m_fileName.c_str()))
		WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "Error, Unable to open the specific script.\n");
	L->selfPtr = this;
}

int Script::GetID() const
{
	return m_nID;
}

void Script::SetUser(User * pUser)
{
	m_pUser = pUser;
}

User * Script::GetUser()
{
	return m_pUser;
}

/*std::thread * Script::GetThread()
{
	return m_pThread;
}

void Script::SetThread(std::thread * pThread)
{
	m_pThread = pThread;
}*/

std::vector<int>& Script::GetArrayObj()
{
	return m_aArrayObj;
}

const std::string & Script::GetUserStringInput() const
{
	return m_strUserInput;
}

int Script::GetUserIntInput() const
{
	return m_nUserInput;
}


void Script::Run()
{
	//m_bResume = 1代表被yield的腳本繼續執行並且把對話結果壓入堆疊中
	auto nResult = lua_resume(L, C, m_bResume ? 1 : 0);
	m_bResume = false;
	if (nResult == LUA_OK || nResult != LUA_YIELD)
		Abort();
}

void Script::Abort()
{
	m_pUser->SetScript(nullptr);
	if (L) lua_close(L);
	m_bDone = true;
}

bool Script::IsDone()
{
	return m_bDone;
}

void Script::OnPacket(InPacket * iPacket)
{
	char nMsgType = iPacket->Decode1(), nAction = 0;
	switch (nMsgType)
	{
		case ScriptType::OnSay:
		{
			nAction = iPacket->Decode1();
			if (nAction == (char)0xFF)
				Abort();
			else if (nAction == 0)
				m_nUserInput = 0;
			else if (nAction == 1)
				m_nUserInput = 1;
			break;
		}
		case ScriptType::OnAskYesNo:
		{
			nAction = iPacket->Decode1();
			if (nAction == (char)0xFF)
				Abort();
			else if (nAction == 0)
				m_nUserInput = 0;
			else if (nAction == 1)
				m_nUserInput = 1;
			lua_pushinteger(L, m_nUserInput);
			m_bResume = true;
			break;
		}
		case ScriptType::OnAskText:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0)
				Abort();
			else if (nAction == 1) 
				lua_pushstring(L, iPacket->DecodeStr().c_str());
			m_bResume = true;
			break;
		}
		case ScriptType::OnAskNumber:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0)
				Abort();
			else if (nAction == 1)
				m_nUserInput = iPacket->Decode4();
			lua_pushinteger(L, m_nUserInput);
			m_bResume = true;
			break;
		}
		case ScriptType::OnAskMenu:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0)
				Abort();
			else if (nAction == 1)
				m_nUserInput = iPacket->Decode4();
			lua_pushinteger(L, m_nUserInput);
			m_bResume = true;
			break;
		}
		case ScriptType::OnAskAvatar:
		{
			iPacket->Decode1();
			iPacket->Decode1();
			nAction = iPacket->Decode1();
			if (nAction == 0)
				Abort();
			else if (nAction == 1) 
			{
				m_nUserInput = iPacket->Decode1();
				if (m_nUserInput < m_aArrayObj.size()) 
					m_pUser->SendCharacterStat(false, QWUser::SetHair(m_pUser, m_aArrayObj[m_nUserInput]));
				m_aArrayObj.clear();
			}
			lua_pushinteger(L, m_nUserInput);
			m_bResume = true;
			break;
		}
	}
}

Script::~Script()
{
}
