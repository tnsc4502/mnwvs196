#pragma once
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Net\WorldInfo.h"
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

	void RegisterChannel(std::shared_ptr<SocketBase> &pServer, InPacket *iPacket);

	ChannelEntry& GetChannel(int idx)
	{
		return aChannel[idx];
	}

	int GetChannelCount()
	{
		return nConnectedChannel;
	}

	const WorldInfo& GetWorldInfo() const
	{
		return mWorldInfo;
	}

	void Init();

	void OnNotifySocketDisconnected(SocketBase *pSocket);
	void NotifyWorldChanged();
};

