#pragma once
#include "Net\asio.hpp"
#include "Net\WvsBase.h"
#include "WvsLoginConstants.hpp"
#include "Center.h"
#include <thread>

class WvsLogin : public WvsBase
{
private:
	void ConnectToCenter(int nCenterIdx);

	std::shared_ptr<Center> aCenterList[WvsLoginConstants::kMaxNumberOfCenters];
	std::shared_ptr<asio::io_service> aCenterServerService[WvsLoginConstants::kMaxNumberOfCenters];
	std::shared_ptr<std::thread> aCenterWorkThread[WvsLoginConstants::kMaxNumberOfCenters];

	void CenterAliveMonitor();

public:

	WvsLogin();
	~WvsLogin();

	int GetCenterCount() const { return WvsLoginConstants::kMaxNumberOfCenters; }

	std::shared_ptr<Center>& GetCenter(int idx)
	{
		return aCenterList[idx];
	}

	void InitializeCenter();

	void OnNotifySocketDisconnected(SocketBase *pSocket);
};

