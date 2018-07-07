#pragma once
#include "..\WvsLib\Net\WvsBase.h"
#include "ChannelEntry.h"

#include <map>

class WvsCenter : public WvsBase
{
private:
	//ChannelEntry aChannel[30];
	std::map<int, ChannelEntry*> m_mChannel;

	int nConnectedChannel = 0;

public:

	WvsCenter();
	~WvsCenter();

	void RegisterChannel(std::shared_ptr<SocketBase> &pServer, InPacket *iPacket);

	ChannelEntry* GetChannel(int idx);
	int GetChannelCount();

	void Init();
	void OnNotifySocketDisconnected(SocketBase *pSocket);
	void NotifyWorldChanged();
};

