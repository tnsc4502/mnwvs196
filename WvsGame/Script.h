#pragma once
#include <iostream>
#include <vector>
#include <mutex>
#include <map>

// Lua is written in C, so compiler needs to know how to link its libraries

#include "..\WvsLib\Script\lapi.h"
#include "..\WvsLib\Script\lua.h"
#include "..\WvsLib\Script\lauxlib.h"
#include "..\WvsLib\Script\lualib.h"
#include "..\WvsLib\Script\luawrapper.hpp"

class User;
class InPacket;
class OutPacket;

class Script
{
public:
	struct NPCConverstaionInfo
	{
		std::string m_sTalkText;
		int m_nMsgType, 
			m_nSpeakerTypeID = 4, 
			m_nSpeakerTemplateID, 
			m_nSpeakerTemplateID_, 
			m_tWait = 0, 
			m_nPage = 0;
		unsigned char m_nParam = 0, m_eColor = 0;

		std::vector<std::string> m_aStrObj;
		std::vector<int> m_aIntObj;
	};

	struct NPCConversationState
	{
		std::vector<NPCConverstaionInfo> m_aPageStack;
		int m_nCurPage = 0, m_nUserInput = 0;
		std::string m_strUserInput;
		bool m_bResume = false, m_bPaging = false;
		std::map<std::string, std::string> m_mAttribute;
	};

	friend class ScriptMan;
	void *m_pUniqueScriptNpc = nullptr;

private:
	lua_State* L, *C; //L = Basic Lua State, C = Coroutine State
	NPCConversationState m_sState;
	NPCConverstaionInfo m_sLastConversationInfo;

	int m_nID;
	std::string m_fileName;

	User *m_pUser;
	bool m_bDone = false;

	void(*m_pOnPacketInvoker)(InPacket*, Script*, lua_State*);

public:
	Script(const std::string& file, int nNpcIDconst, const std::vector<void(*)(lua_State*)>& aReg);
	~Script();
	static Script* GetSelf(lua_State * L);
	static void DestroySelf(lua_State * L, Script* p);

	static void Register(lua_State* L);
	void Wait();
	void Notify();
	void Run();
	void Abort();
	bool IsDone();
	bool Init();
	void OnPacket(InPacket *iPacket);

	int GetID() const; 
	void SetUser(User *pUser);
	User* GetUser();
	lua_State* GetLuaState();
	lua_State* GetLuaCoroutineState();

	NPCConversationState& GetConverstaionState();

	void SetLastConversationInfo(const NPCConverstaionInfo& refInfo);
	NPCConverstaionInfo& GetLastConversationInfo();
	
	template<typename T>
	void RetrieveArray(std::vector<T>& out, int nLuaObjIndex);

	template<>
	void RetrieveArray<std::string>(std::vector<std::string>& out, int nLuaObjIndex);

	template<typename T>
	void PushClassObject(T *pObj);
};

template<typename T>
inline void Script::RetrieveArray(std::vector<T>& out, int nLuaObjIndex)
{
	int nSize = (int)lua_rawlen(L, nLuaObjIndex);
	for (int i = 0; i < nSize; ++i)
	{
		lua_pushinteger(L, i + 1);
		lua_gettable(L, -2);
		out.push_back((T)lua_tonumber(L, -1));
		lua_pop(L, 1);
	}
}

template<>
inline void Script::RetrieveArray<std::string>(std::vector<std::string>& out, int nLuaObjIndex)
{
	int nSize = (int)lua_rawlen(L, nLuaObjIndex);
	for (int i = 0; i < nSize; ++i)
	{
		lua_pushinteger(L, i + 1);
		lua_gettable(L, -2);
		out.push_back(lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

template<typename T>
inline void Script::PushClassObject(T *pObj)
{
	int numargs = lua_gettop(L);
	luaW_push<T>(L, pObj); // ... args... ud
	luaW_hold<T>(L, pObj);
	lua_insert(L, -1 - numargs); // ... ud args...
	luaW_postconstructor<T>(L, numargs); // ... ud
}