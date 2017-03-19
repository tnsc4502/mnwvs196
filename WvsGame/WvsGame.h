#pragma once
#include "Net\WvsBase.h"
#include "Net\WorldInfo.h"

#include "Center.h"
#include <thread>

class WvsGame : public WvsBase
{
	std::shared_ptr<Center> aCenterList;
	asio::io_service* aCenterServerService;
	std::thread* aCenterWorkThread;
public:
	WvsGame();
	~WvsGame();

	void ConnectToCenter(int nCenterIdx, WorldConnectionInfo& cInfo);
	void WvsGame::InitializeCenter();

	void OnNotifySocketDisconnected(SocketBase *pSocket){}
};