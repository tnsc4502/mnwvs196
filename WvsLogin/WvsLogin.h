#pragma once
#include "Net\asio.hpp"
#include "Net\WvsBase.h"
#include "WvsLoginConstants.hpp"
#include "Center.h"
#include <thread>

class WvsLogin : public WvsBase
{
private:
	void ConnectToCenter(int nCenterIdx, WorldConnectionInfo& cInfo);

	std::shared_ptr<Center> aCenterList[WvsLoginConstants::kMaxNumberOfCenters];
	asio::io_service* aCenterServerService[WvsLoginConstants::kMaxNumberOfCenters];
	std::thread* aCenterWorkThread[WvsLoginConstants::kMaxNumberOfCenters];

public:

	WvsLogin();
	~WvsLogin();

	std::shared_ptr<Center>& GetCenter(int idx)
	{
		return aCenterList[idx];
	}

	void InitializeCenter();
};

