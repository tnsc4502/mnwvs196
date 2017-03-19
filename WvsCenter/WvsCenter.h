#pragma once
#include "Net\asio.hpp"
#include "Net\WvsBase.h"
#include "Net\WorldInfo.h"
#include "ChannelEntry.h"

class WvsCenter : public WvsBase
{
private:
	WorldInfo mWorldInfo;

	ChannelEntry aChannel[30];

	int nConnectedChannel = 0;

public:

	WvsCenter();
	~WvsCenter();

	void RegisterChannel(std::shared_ptr<SocketBase> &pServer)
	{
		aChannel[nConnectedChannel++].SetGameServer(pServer);
	}

	ChannelEntry& GetChannel(int idx)
	{
		return aChannel[idx];
	}

	int GetChannelCount()
	{
		return nConnectedChannel;
	}

	void OnNotifySocketDisconnected(SocketBase *pSocket);
	void NotifyWorldChanged();
};

