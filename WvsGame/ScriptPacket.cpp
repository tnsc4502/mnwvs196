#include "ScriptPacket.h"
#include "Script.h"
#include "User.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Net\OutPacket.h"
#include "ScriptUser.h"

ScriptPacket::ScriptPacket()
{
}

ScriptPacket::~ScriptPacket()
{
	if (m_pOutPacket)
		FreeObj(m_pOutPacket);
}

ScriptPacket* ScriptPacket::PacketNew(lua_State * L)
{
	ScriptPacket *p = AllocObj(ScriptPacket);
	p->m_pOutPacket = AllocObj(OutPacket);
	return p;
}

void ScriptPacket::DestroyPacket(lua_State * L, ScriptPacket * p)
{
	FreeObj(p);
}

int ScriptPacket::PacketClear(lua_State * L)
{
	ScriptPacket* self = luaW_check<ScriptPacket>(L, 1);
	self->m_pOutPacket->Reset();
	return 1;
}

int ScriptPacket::PacketEncode1(lua_State * L)
{
	ScriptPacket* self = luaW_check<ScriptPacket>(L, 1);
	int nOut = (int)luaL_checkinteger(L, 2);
	self->m_pOutPacket->Encode1((char)nOut);
	return 1;
}

int ScriptPacket::PacketEncode2(lua_State * L)
{
	ScriptPacket* self = luaW_check<ScriptPacket>(L, 1);
	int nOut = (int)luaL_checkinteger(L, 2);
	self->m_pOutPacket->Encode2((short)nOut);
	return 1;
}

int ScriptPacket::PacketEncode4(lua_State * L)
{
	ScriptPacket* self = luaW_check<ScriptPacket>(L, 1);
	int nOut = (int)luaL_checkinteger(L, 2);
	self->m_pOutPacket->Encode4(nOut);
	return 1;
}

int ScriptPacket::PacketEncode8(lua_State * L)
{
	ScriptPacket* self = luaW_check<ScriptPacket>(L, 1);
	long long int nOut = (long long int)luaL_checkinteger(L, 2);
	self->m_pOutPacket->Encode8(nOut);
	return 1;
}

int ScriptPacket::PacketEncodeStr(lua_State * L)
{
	ScriptPacket* self = luaW_check<ScriptPacket>(L, 1);
	auto sStr = luaL_checkstring(L, 2);
	self->m_pOutPacket->EncodeStr(sStr);
	return 1;
}

int ScriptPacket::PacketEncodeHex(lua_State * L)
{
	ScriptPacket* self = luaW_check<ScriptPacket>(L, 1);
	auto sStr = luaL_checkstring(L, 2);
	self->m_pOutPacket->EncodeHexString(sStr);
	return 1;
}

int ScriptPacket::PacketPrint(lua_State * L)
{
	ScriptPacket* self = luaW_check<ScriptPacket>(L, 1);
	self->m_pOutPacket->Print();
	return 1;
}

int ScriptPacket::PacketSend(lua_State * L)
{
	ScriptPacket* self = luaW_check<ScriptPacket>(L, 1);

	//int nUserID = luaL_checkinteger(L, 2);
	//auto pUser = User::FindUser(nUserID);
	auto pUser = luaW_check<ScriptUser>(L, 2);
	if (pUser && pUser->GetUser())
		pUser->GetUser()->SendPacket(self->m_pOutPacket);
	return 1;
}

void ScriptPacket::Register(lua_State * L)
{
	luaL_Reg PacketMetatable[] = {
		{ "clear", PacketClear },
		{ "encode1", PacketEncode1 },
		{ "encode2", PacketEncode2 },
		{ "encode4", PacketEncode4 },
		{ "encode8", PacketEncode8 },
		{ "encodeStr", PacketEncodeStr },
		{ "encodeHex", PacketEncodeHex },
		{ "print", PacketPrint },
		{ "send", PacketSend },
		{ NULL, NULL }
	};

	luaL_Reg PacketTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptPacket>(L, "Packet", 
		PacketTable, 
		PacketMetatable, 
		&(ScriptPacket::PacketNew),
		&(ScriptPacket::DestroyPacket));
}
