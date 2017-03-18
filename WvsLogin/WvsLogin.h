#pragma once
#include "Net\asio.hpp"
#include "Net\WvsBase.h"
#include "WvsLoginConstants.hpp"
#include "Center.h"
#include <thread>

class WvsLogin : public WvsBase
{
private:
	void ConnectToCenter(int nCenterIdx, const WvsLoginConstants::CenterServerInfo& cInfo);

public:
	std::shared_ptr<Center> aCenterList[WvsLoginConstants::kMaxNumberOfCenters];
	asio::io_service* aCenterServerService[WvsLoginConstants::kMaxNumberOfCenters];
	std::thread* aCenterWorkThread[WvsLoginConstants::kMaxNumberOfCenters];

	WvsLogin();
	~WvsLogin();

	void InitializeCenter();
};

