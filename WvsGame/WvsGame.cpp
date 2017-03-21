#include "WvsGame.h"
#include "WvsGameConstants.h"

#include "Constants\ConfigLoader.hpp"

WvsGame::WvsGame()
{

}

WvsGame::~WvsGame()
{

}

void WvsGame::ConnectToCenter(int nCenterIdx, WorldConnectionInfo& cInfo)
{
	aCenterServerService = new asio::io_service();
	aCenterList = std::make_shared<Center>(*aCenterServerService);
	aCenterList->SetDisconnectedNotifyFunc(OnSocketDisconnected);
	aCenterList->SetCenterIndex(nCenterIdx);
	aCenterList->OnConnectToCenter(cInfo.strServerIP, cInfo.nServerPort);
	asio::io_service::work work(*aCenterServerService);
	std::error_code ec;
	aCenterServerService->run(ec);
}

void WvsGame::InitializeCenter()
{
	WvsGameConstants::CenterServerList[0].nServerPort = ConfigLoader::GetInstance()->IntValue("Center0_Port");
	WvsGameConstants::CenterServerList[0].strServerIP = ConfigLoader::GetInstance()->StrValue("Center0_IP");
	auto& aCenterInfoList = WvsGameConstants::CenterServerList;
	//int centerSize = sizeof(aCenterInfoList) / sizeof(aCenterInfoList[0]);
	//for (int i = 0; i < centerSize; ++i)
	aCenterWorkThread = new std::thread(&WvsGame::ConnectToCenter, this, 0, aCenterInfoList[0]);
}