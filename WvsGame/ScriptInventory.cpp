#include "ScriptInventory.h"
#include "QWUInventory.h"
#include "Script.h"
#include "User.h"

#include "..\WvsLib\Logger\WvsLogger.h"

ScriptInventory::ScriptInventory()
{
}


ScriptInventory::~ScriptInventory()
{
}

void ScriptInventory::Register(lua_State * L)
{
	luaL_Reg InvMetatable[] = {
		{ NULL, NULL }
	};

	luaL_Reg InvTable[] = {
		{ "exchange", InventoryExchange },
		{ NULL, NULL }
	};

	luaW_register<Script>(L, "inventory", InvTable, InvMetatable, &(Script::GetSelf));
}

int ScriptInventory::InventoryExchange(lua_State * L)
{
	Script *self = (Script*)L->selfPtr;
	int nMoney = (int)luaL_checkinteger(L, 1), nItemID, nCount;
	int nArg = lua_gettop(L);
	std::vector<ExchangeElement> aExchange;
	std::vector<InventoryManipulator::ChangeLog> aLogAdd, aLogRemove;
	for (int i = 2; i <= nArg - 1;)
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
	int nResult = QWUInventory::Exchange(self->GetUser(), nMoney, aExchange, aLogAdd, aLogRemove);
	/*
	0 = Success
	1 = Meso Insufficient
	2 = No Available Slot
	3 = Insufficient Item In The Slot
	*/


	lua_pushinteger(L, nResult);
	return 1;
}
