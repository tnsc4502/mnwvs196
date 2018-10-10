#pragma once
struct GW_ItemSlotBase;

struct ExchangeElement
{
	int m_nItemID = 0, m_nCount = 0;
	GW_ItemSlotBase* m_pItem = nullptr;
};

