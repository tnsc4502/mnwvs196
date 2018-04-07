#pragma once
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>

// Lua is written in C, so compiler needs to know how to link its libraries

#include "..\ScriptLib\lapi.h"
#include "..\ScriptLib\lua.h"
#include "..\ScriptLib\lauxlib.h"
#include "..\ScriptLib\lualib.h"


#include "..\ScriptLib\luawrapper.hpp"

class User;
class InPacket;
class OutPacket;

class Script
{
	enum ScriptType
	{
		OnSay = 0x00,
		OnSayImage = 0x02,
		OnAskYesNo = 0x03,
		OnAskText = 0x04,
		OnAskNumber = 0x05,
		OnAskMenu = 0x06,
		OnAskQuiz = 0xFF,
		OnAskSpeedQuiz = 0xFF,
		OnAskAvatar = 0x0B,
		OnAskAvatarZero = 0x26,
		OnAskMixHair = 0x2C,
		OnAskMixHairExZero = 0x2D,
		OnAskCustomMixHairAndProb = 0x2F,
		OnAskMixHairNew = 0x30,
		OnAskMixHairNewExZero = 0x30,
		OnAskAndroid = 0x0C,
		OnAskPet = 0x0D,
		OnAskPetAll = 0x0E,
		OnAskActionPetEvolution = 0x0F,
		OnInitialQuiz = 0x08,
		OnInitialSpeedQuiz = 0x09,
		OnICQuiz = 0x0A,
		OnAskAcceptDecline = 0x11,
		OnAskBoxText = 0xFF,
		OnAskSlideMenu = 0x14,
		OnAskSelectMenu = 0x1B,
		OnAskAngelicBuster = 0x1C,
		OnSayIllustration = 0x1D, //1D 1E
		OnAskYesNoIllustration = 0x1F, //0x1F 0x20 0x22 0x23
		OnAskMenuIllustration = 0x21, //0x21, 0x24
		OnAskWeaponBox = 0x28,
		OnAskUserSurvey = 0x2A,
		OnAskScreenShinningStarMsg = 0x33,
		OnAskNumberUseKeyPad = 0x36,
		OnSpinOffGuitarRhythmGame = 0x37,
		OnGhostParkEnter = 0x38,
	};

	friend class ScriptMan;

	lua_State* L;
	//std::unique_ptr<lua_State, void(*)(lua_State*)> L;
	std::mutex m_mtxWaitLock;
	std::condition_variable m_cndVariable, m_doneVariable;

	static int SelfSayNextGroup(lua_State* L);
	static int SelfSay(lua_State* L);
	static int SelfAskAvatar(lua_State* L);
	static int SelfSayNext(lua_State* L);
	static int SelfAskText(lua_State* L);
	static int SelfAskNumber(lua_State* L);
	static int SelfAskYesNo(lua_State* L);
	static int SelfAskMenu(lua_State* L);
	static int SelfPushArray(lua_State* L);
	static Script* GetSelf(lua_State* L);

	static luaL_Reg SelfTable[];
	static luaL_Reg SelfMetatable[];

	int m_nID, m_nUserInput;
	std::string m_fileName, m_strUserInput;
	User *m_pUser;
	std::thread* m_pThread;
	std::vector<int> m_aArrayObj;
	bool m_bDone = false;

public:
	void Wait();
	void Notify();
	void Run();
	void Abort();
	bool IsDone();

	void OnPacket(InPacket *iPacket);

	Script(const std::string& file, int nNpcID);
	~Script();

	static int LuaRegisterSelf(lua_State* L);

	void SetUser(User *pUser);

	std::thread* GetThread();
	void SetThread(std::thread* pThread);
};