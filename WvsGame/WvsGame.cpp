#include "WvsGame.h"
#include "WvsGameConstants.h"
#include "Constants\ConfigLoader.hpp"
#include "ClientSocket.h"

WvsGame::WvsGame()
{

}

WvsGame::~WvsGame()
{

}

void WvsGame::ConnectToCenter(int nCenterIdx, WorldConnectionInfo& cInfo)
{
	aCenterServerService = new asio::io_service();
	aCenterPtr = std::make_shared<Center>(*aCenterServerService);
	aCenterPtr->SetDisconnectedNotifyFunc(OnSocketDisconnected);
	aCenterPtr->SetCenterIndex(nCenterIdx);
	aCenterPtr->OnConnectToCenter(cInfo.strServerIP, cInfo.nServerPort);
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

void WvsGame::OnUserConnected(std::shared_ptr<User> &pUser)
{
	mUserMap[pUser->GetUserID()] = pUser;
}

void WvsGame::OnNotifySocketDisconnected(SocketBase *pSocket)
{
	auto pClient = (ClientSocket*)pSocket;
	mUserMap.erase(pClient->GetUser()->GetUserID());
}