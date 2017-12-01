#include "WvsGame.h"
#include "WvsGameConstants.h"
#include "Task\AsnycScheduler.h"
#include "Constants\ConfigLoader.hpp"
#include "ClientSocket.h"


WvsGame::WvsGame()
{

}

WvsGame::~WvsGame()
{

}

void WvsGame::ConnectToCenter(int nCenterIdx)
{
	aCenterServerService = new asio::io_service();
	aCenterPtr = std::make_shared<Center>(*aCenterServerService);
	aCenterPtr->SetDisconnectedNotifyFunc(&Center::OnNotifyCenterDisconnected);
	aCenterPtr->SetCenterIndex(nCenterIdx);
	aCenterPtr->OnConnectToCenter(
		ConfigLoader::GetInstance()->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		ConfigLoader::GetInstance()->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);
	asio::io_service::work work(*aCenterServerService);
	std::error_code ec;
	aCenterServerService->run(ec);
}

void WvsGame::CenterAliveMonitor()
{
	printf("=================定期檢查Center Server連線程序=================\n");
	if (aCenterPtr && aCenterPtr->GetWorldInfo().bConnectionFailed) 
	{
		printf("Center Server %d 尚未連線，嘗試重新連線。\n", 0);
		aCenterPtr.reset();
		aCenterServerService->stop();
		aCenterWorkThread->detach();

		new std::thread(&WvsGame::ConnectToCenter, this, 0);
	}
}

void WvsGame::InitializeCenter()
{
	//WvsGameConstants::CenterServerList[0].nServerPort = ConfigLoader::GetInstance()->IntValue("Center0_Port");
	//WvsGameConstants::CenterServerList[0].strServerIP = ConfigLoader::GetInstance()->StrValue("Center0_IP");
	//auto& aCenterInfoList = WvsGameConstants::CenterServerList;
	//int centerSize = sizeof(aCenterInfoList) / sizeof(aCenterInfoList[0]);
	//for (int i = 0; i < centerSize; ++i)
	aCenterWorkThread = new std::thread(&WvsGame::ConnectToCenter, this, 0);
	auto holderFunc = std::bind(&WvsGame::CenterAliveMonitor, this);
	auto aliveHolder = AsnycScheduler::CreateTask(holderFunc, 10 * 1000, true);
	aliveHolder->Start();
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

void WvsGame::SetExternalIP(const std::string& ip)
{
	int ipPos = 0, strPos = 0;
	std::string tmpStr = "";
	while (strPos <= ip.size() )
	{
		if (strPos == ip.size() || ip[strPos] == '.')
		{
			aExternalIP[ipPos++] = atoi(tmpStr.c_str());
			tmpStr = "";
		}
		else
			tmpStr += ip[strPos];
		++strPos;
	}
}

void WvsGame::SetExternalPort(short nPort)
{
	nExternalPort = nPort;
}

int* WvsGame::GetExternalIP() const
{
	return (int*)aExternalIP;
}

short WvsGame::GetExternalPort() const
{
	return nExternalPort;
}