#pragma once
#include "..\WvsLib\Net\WvsBase.h"
#include "LocalServerEntry.h"

#include <map>

class WvsCenter : public WvsBase
{
private:
	//ChannelEntry aChannel[30];
	std::map<int, LocalServerEntry*> m_mChannel;
	LocalServerEntry* m_pShopEntry;

	int nConnectedChannel = 0;

public:

	WvsCenter();
	~WvsCenter();

	void RegisterChannel(std::shared_ptr<SocketBase> &pServer, InPacket *iPacket);
	void RegisterCashShop(std::shared_ptr<SocketBase> &pServer, InPacket *iPacket);

	LocalServerEntry* GetChannel(int idx);

	int GetChannelCount();

	void Init();
	void OnNotifySocketDisconnected(SocketBase *pSocket);
	void NotifyWorldChanged();

	LocalServerEntry* GetShop();
	void SetShop(LocalServerEntry* pEntry);
};

