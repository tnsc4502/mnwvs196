#pragma once
#include "FieldObj.h"

class User;
class InPacket;
class NpcTemplate;

class Npc : public FieldObj
{
	NpcTemplate *m_pTemplate;

	void OnShopPurchaseItem(User *pUser, InPacket *iPacket);
	void OnShopSellItem(User *pUser, InPacket *iPacket);
	void OnShopRechargeItem(User *pUser, InPacket *iPacket);
	void MakeShopResult(User *pUser, void* pItem, OutPacket *oPacket, int nAction, int nIdx);

public:
	Npc();
	~Npc();
	void SetTemplate(NpcTemplate *pTemplate);
	NpcTemplate* GetTemplate();

	void OnShopRequest(User *pUser, InPacket *iPacket);

	void OnUpdateLimitedInfo(User* pUser, InPacket *iPacket);
	void SendChangeControllerPacket(User* pUser);
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void EncodeInitData(OutPacket *oPacket);
};

