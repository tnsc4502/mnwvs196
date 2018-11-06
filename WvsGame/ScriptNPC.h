#pragma once
#include "..\WvsLib\Script\lua.h"

class Script;
class InPacket;

class ScriptNPC
{
	enum ScriptStyle
	{
		eNoESC = 0x1,
		ePlayerRespond = 0x02,
		eInverseDirection = 0x08,
		ePlayerTalk = 0x10,
		eFullScreen = 0x20
	};

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
public:
	ScriptNPC();
	~ScriptNPC();

	static ScriptNPC* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptNPC* p);
	static void Register(lua_State* L);
	static void OnPacket(InPacket *iPacket, Script* pScript, lua_State* L);

	static int SelfSay(lua_State* L);
	static int SelfAskAvatar(lua_State* L);
	static int SelfSayNext(lua_State* L);
	static void SelfSayNextImpl(lua_State* L, int nPage);

	static int SelfAskText(lua_State* L);
	static int SelfAskNumber(lua_State* L);
	static int SelfAskYesNo(lua_State* L);
	static int SelfAskAcceptDecline(lua_State* L);
	static int SelfAskMenu(lua_State* L);
	static int Debug(lua_State* L);

	static void CheckMessageParameter(lua_State* L, int nStartIndex, void *pInfo);
	static void MakeMessagePacket(lua_State* L, void *pInfo);
};

