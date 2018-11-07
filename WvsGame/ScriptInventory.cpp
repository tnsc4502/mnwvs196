#include "ScriptInventory.h"
#include "QWUInventory.h"
#include "..\Database\GA_Character.hpp"
#include "Script.h"
#include "User.h"

#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

ScriptInventory::ScriptInventory()
{
}


ScriptInventory::~ScriptInventory()
{
	WvsLogger::LogRaw("Remove Script Inventory.");
}

void ScriptInventory::SetUser(User * pUser)
{
	m_pUser = pUser;
}

ScriptInventory* ScriptInventory::GetSelf(lua_State* L)
{
	int nUserID = (int)luaL_checkinteger(L, 1);
	auto pUser = User::FindUser(nUserID);
	if (!pUser)
		return nullptr;
	ScriptInventory *p = AllocObj(ScriptInventory);
	p->m_pUser = pUser;
	return p;
}

void ScriptInventory::DestroySelf(lua_State * L, ScriptInventory * p)
{
	FreeObj(p);
}

void ScriptInventory::Register(lua_State * L)
{
	luaL_Reg InvMetatable[] = {
		{ "exchange", InventoryExchange },
		{ "itemCount", InventoryItemCount },
		{ NULL, NULL }
	};

	luaL_Reg InvTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptInventory>(L, "Inventory", 
		InvTable, 
		InvMetatable, 
		&(ScriptInventory::GetSelf),
		&(ScriptInventory::DestroySelf));
}

int ScriptInventory::InventoryExchange(lua_State * L)
{
	ScriptInventory* self = luaW_check<ScriptInventory>(L, 1);
	int nMoney = (int)luaL_checkinteger(L, 2), nItemID, nCount;
	int nArg = lua_gettop(L);
	std::vector<ExchangeElement> aExchange;
	std::vector<InventoryManipulator::ChangeLog> aLogAdd, aLogRemove;
	for (int i = 3; i <= nArg - 1;)
	{
		nItemID = (int)lua_tointeger(L, i);
		nCount = (int)lua_tointeger(L, i + 1);
		ExchangeElement e;
		e.m_nItemID = nItemID;
		e.m_nCount = nCount;
		e.m_pItem = nullptr;
		aExchange.push_back(std::move(e));
		i += 2;
	}
	int nResult = QWUInventory::Exchange(self->m_pUser, nMoney, aExchange, aLogAdd, aLogRemove);
	/*
	0 = Success
	1 = Meso Insufficient
	2 = No Available Slot
	3 = Insufficient Item In The Slot
	*/


	lua_pushinteger(L, nResult);
	return 1;
}

int ScriptInventory::InventoryItemCount(lua_State * L)
{
	ScriptInventory* self = luaW_check<ScriptInventory>(L, 1);
	int nItemID = (int)luaL_checkinteger(L, 2);
	int nCount = self->m_pUser->GetCharacterData()->GetItemCount(nItemID / 1000000, nItemID);
	lua_pushinteger(L, nCount);
	return 0;
}
