#pragma once
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>

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

private:
	lua_State* L, *C;

	int m_nID, m_nUserInput;
	std::string m_fileName, m_strUserInput;
	User *m_pUser;
	std::thread* m_pThread;
	std::vector<int> m_aArrayObj;
	bool m_bDone = false;
	bool m_bResume = false;

public:
	void Wait();
	void Notify();
	void Run();
	void Abort();
	bool IsDone();

	void OnPacket(InPacket *iPacket);

	Script(const std::string& file, int nNpcIDconst, const std::vector<void(*)(lua_State*)>& aReg);
	~Script();
	static Script* GetSelf(lua_State * L);

	int GetID() const; 

	void SetUser(User *pUser);
	User* GetUser();

	std::thread* GetThread();
	void SetThread(std::thread* pThread);

	std::vector<int>& GetArrayObj();
	const std::string& GetUserStringInput() const;
	int GetUserIntInput() const;
};