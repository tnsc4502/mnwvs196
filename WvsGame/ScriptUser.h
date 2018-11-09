#pragma once
#include "..\WvsLib\Script\lua.h"

class User;

class ScriptUser
{
	User *m_pUser;

public:
	ScriptUser();
	~ScriptUser();

	User* GetUser();
	static ScriptUser* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptUser* p);
	static void Register(lua_State* L);
	static int TargetChatMessage(lua_State* L);
	static int TargetNoticeMessage(lua_State* L);
	static int TargetInventory(lua_State* L);
	static int TargetQuestRecord(lua_State* L);
	static int TargetRegisterTransferField(lua_State* L);
	static int TargetGetPosX(lua_State* L);
	static int TargetGetPosY(lua_State* L);
	static int TargetIncHP(lua_State* L);
	static int TargetIncMP(lua_State* L);
	static int TargetIncSTR(lua_State* L);
	static int TargetIncINT(lua_State* L);
	static int TargetIncDEX(lua_State* L);
	static int TargetIncLUK(lua_State* L);
	static int TargetIncSP(lua_State* L);
	static int TargetIncAP(lua_State* L);
	static int TargetIncMMP(lua_State* L);
	static int TargetIncMHP(lua_State* L); 
	static int TargetIncEXP(lua_State* L);
	static int TargetIncMoney(lua_State* L);

	static int TargetGetHP(lua_State* L);
	static int TargetGetMP(lua_State* L);
	static int TargetGetSTR(lua_State* L);
	static int TargetGetINT(lua_State* L);
	static int TargetGetDEX(lua_State* L);
	static int TargetGetLUK(lua_State* L);
	static int TargetGetSP(lua_State* L);
	static int TargetGetAP(lua_State* L);
	static int TargetGetMMP(lua_State* L);
	static int TargetGetMHP(lua_State* L);
	static int TargetGetEXP(lua_State* L);
	static int TargetGetMoney(lua_State* L);
	static int TargetIsWearing(lua_State* L);
};

