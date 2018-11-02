#pragma once
#include "..\WvsLib\Script\lua.h"

class OutPacket;

class ScriptPacket
{
	OutPacket *m_pOutPacket = nullptr;

public:
	ScriptPacket();
	~ScriptPacket();

	static ScriptPacket* PacketNew(lua_State* L);
	static void DestroyPacket(lua_State* L, ScriptPacket *p);
	static int PacketClear(lua_State* L);
	static int PacketEncode1(lua_State* L);
	static int PacketEncode2(lua_State* L);
	static int PacketEncode4(lua_State* L);
	static int PacketEncode8(lua_State* L);
	static int PacketEncodeStr(lua_State* L);
	static int PacketEncodeHex(lua_State* L);
	static int PacketPrint(lua_State* L);
	static int PacketSend(lua_State* L);

	static void Register(lua_State* L);
};

