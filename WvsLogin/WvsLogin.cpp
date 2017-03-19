#include "WvsLogin.h"
#include <thread>
#include <iostream>
#include "Net\OutPacket.h"

WvsLogin::WvsLogin()
{
}


WvsLogin::~WvsLogin()
{
}

void WvsLogin::ConnectToCenter(int nCenterIdx, WorldConnectionInfo& cInfo)
{
	aCenterServerService[nCenterIdx] = new asio::io_service();		
	aCenterList[nCenterIdx] = std::make_shared<Center>(*aCenterServerService[nCenterIdx]);
	aCenterList[nCenterIdx]->SetDisconnectedNotifyFunc(OnSocketDisconnected);
	aCenterList[nCenterIdx]->SetCenterIndex(nCenterIdx);
	aCenterList[nCenterIdx]->OnConnectToCenter(cInfo.strServerIP, cInfo.nServerPort); 
	asio::io_service::work work(*aCenterServerService[nCenterIdx]);
	std::error_code ec;
	aCenterServerService[nCenterIdx]->run(ec);
}

void WvsLogin::InitializeCenter()
{
	auto& aCenterInfoList = WvsLoginConstants::CenterServerList;
	int centerSize = sizeof(aCenterInfoList) / sizeof(aCenterInfoList[0]);
	for (int i = 0; i < centerSize; ++i)
		aCenterWorkThread[i] = new std::thread(&WvsLogin::ConnectToCenter, this, i, aCenterInfoList[i]);
}