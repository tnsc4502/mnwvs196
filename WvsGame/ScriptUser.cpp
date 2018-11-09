#include "ScriptUser.h"
#include "ScriptInventory.h"
#include "ScriptQuestRecord.h"
#include "Script.h"
#include "User.h"
#include "QWUser.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GA_Character.hpp"

ScriptUser::ScriptUser()
{
}

ScriptUser::~ScriptUser()
{
}

User * ScriptUser::GetUser()
{
	return m_pUser;
}

ScriptUser * ScriptUser::GetSelf(lua_State * L)
{
	int nUserID = (int)luaL_checkinteger(L, 1);
	auto pUser = User::FindUser(nUserID);
	if (pUser == nullptr)
		return nullptr;
	ScriptUser* p = AllocObj(ScriptUser);
	p->m_pUser = pUser;
	return p;
}

void ScriptUser::DestroySelf(lua_State * L, ScriptUser * p)
{
	FreeObj(p);
}

void ScriptUser::Register(lua_State * L)
{
	luaL_Reg TargetMetatable[] = {
		{ "noticeMsg", TargetNoticeMessage },
		{ "chatMsg", TargetChatMessage },
		{ "inventory", TargetInventory }, 
		{ "questRecord", TargetQuestRecord },
		{ "transferField", TargetRegisterTransferField },
		{ "incHP", TargetIncHP },
		{ "incMP", TargetIncMP },
		{ "incSTR", TargetIncSTR },
		{ "incINT", TargetIncINT },
		{ "incDEX", TargetIncDEX },
		{ "incLUK", TargetIncLUK },
		{ "incSP", TargetIncSP },
		{ "incAP", TargetIncAP },
		{ "incMMP", TargetIncMMP },
		{ "incMHP", TargetIncMHP },
		{ "incEXP", TargetIncEXP },
		{ "incMoney", TargetIncMoney },
		{ "getHP", TargetGetHP },
		{ "getMP", TargetGetMP },
		{ "getSTR", TargetGetSTR },
		{ "getINT", TargetGetINT },
		{ "getDEX", TargetGetDEX },
		{ "getLUK", TargetGetLUK },
		{ "getSP", TargetGetSP },
		{ "getAP", TargetGetAP },
		{ "getMMP", TargetGetMMP },
		{ "getMHP", TargetGetMHP },
		{ "getEXP", TargetGetEXP },
		{ "getMoney", TargetGetMoney },
		{ "isWearing", TargetIsWearing },
		{ "getPosX", TargetGetPosX },
		{ "getPosY", TargetGetPosY },
		{ NULL, NULL }
	};

	luaL_Reg TargetTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptUser>(L, "User", 
		TargetTable, 
		TargetMetatable, 
		&(ScriptUser::GetSelf),
		&(ScriptUser::DestroySelf));
}

int ScriptUser::TargetChatMessage(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nType = (int)luaL_checkinteger(L, 2);
	const char* sMsg = luaL_checkstring(L, 3);
	self->m_pUser->SendChatMessage(nType, sMsg);
	lua_pushinteger(L, 1);
	return 1;
}

int ScriptUser::TargetNoticeMessage(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nType = (int)luaL_checkinteger(L, 2);
	const char* sMsg = luaL_checkstring(L, 3);
	self->m_pUser->SendNoticeMessage(nType, sMsg);
	lua_pushinteger(L, 1);
	return 1;
}

int ScriptUser::TargetInventory(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	ScriptInventory* obj = (self->m_pUser == nullptr ? nullptr : AllocObj(ScriptInventory));
	if (obj)
		obj->SetUser(self->m_pUser);

	((Script*)L->selfPtr)->PushClassObject(obj);
	return 1;
}

int ScriptUser::TargetQuestRecord(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	ScriptQuestRecord* obj = 
		(self->m_pUser == nullptr ? nullptr : AllocObj(ScriptQuestRecord));
	if (obj)
		obj->SetUser(self->m_pUser);

	((Script*)L->selfPtr)->PushClassObject(obj);
	return 1;
}

int ScriptUser::TargetRegisterTransferField(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nFieldID = (int)luaL_checkinteger(L, 2);
	const char *sPortal = luaL_checkstring(L, 3);
	self->GetUser()->TryTransferField(nFieldID, sPortal);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetGetPosX(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushinteger(L, self->m_pUser->GetPosX());
	return 1;
}

int ScriptUser::TargetGetPosY(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushinteger(L, self->m_pUser->GetPosY());
	return 1;
}

int ScriptUser::TargetIncHP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);
	
	auto liFlag = QWUser::IncHP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncMP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncMP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncSTR(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncSTR(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncINT(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncINT(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncDEX(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncDEX(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncLUK(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncLUK(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncSP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);
	int nJobLevel = (int)(lua_gettop(L) > 3 ? luaL_checkinteger(L, 4) : -1);

	auto liFlag = QWUser::IncSP(
		self->m_pUser, 
		(nJobLevel != -1 ? nJobLevel : 
			WvsGameConstants::GetJobLevel(
				self->m_pUser->GetCharacterData()->mStat->nJob
			)
		),
		nVal, 
		nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncAP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncAP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncMMP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncMMP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncMHP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncMHP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncEXP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncEXP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncMoney(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncMoney(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetGetHP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetHP(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetMP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetMP(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetSTR(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetSTR(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetINT(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetINT(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetDEX(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetDEX(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetLUK(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetLUK(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetSP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nJobLevel = (int)(lua_gettop(L) > 1 ? luaL_checkinteger(L, 2) : -1);

	lua_pushinteger(L, QWUser::GetSP(
		self->m_pUser,
		nJobLevel != -1 ? 
		nJobLevel :
		WvsGameConstants::GetJobLevel(
			self->m_pUser->GetCharacterData()->mStat->nJob
		)
	));
	return 1;
}

int ScriptUser::TargetGetAP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetAP(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetMMP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetMaxMPVal(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetMHP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetMaxHPVal(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetEXP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetEXP(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetMoney(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetMoney(self->m_pUser));
	return 1;
}

int ScriptUser::TargetIsWearing(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nItemID = (int)luaL_checkinteger(L, 2);

	lua_pushinteger(L, self->m_pUser->GetCharacterData()->IsWearing(nItemID));
	return 1;
}
