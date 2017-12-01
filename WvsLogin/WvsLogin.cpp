#include "WvsLogin.h"
#include <thread>
#include <iostream>
#include <functional>
#include "Net\OutPacket.h"
#include "Task\AsnycScheduler.h"

#include "Constants\ConfigLoader.hpp"
#include "Constants\ServerConstants.hpp"

WvsLogin::WvsLogin()
{
}


WvsLogin::~WvsLogin()
{
}

void WvsLogin::ConnectToCenter(int nCenterIdx)
{
	aCenterServerService[nCenterIdx].reset( new asio::io_service() );		
	aCenterList[nCenterIdx] = std::make_shared<Center>(*aCenterServerService[nCenterIdx]);
	aCenterList[nCenterIdx]->SetDisconnectedNotifyFunc(&Center::OnNotifyCenterDisconnected);
	aCenterList[nCenterIdx]->SetCenterIndex(nCenterIdx);
	aCenterList[nCenterIdx]->OnConnectToCenter(
		ConfigLoader::GetInstance()->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		ConfigLoader::GetInstance()->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);
	//if (aCenterList[nCenterIdx]->IsConnected()) {
		asio::io_service::work work(*aCenterServerService[nCenterIdx]);
		std::error_code ec;
		aCenterServerService[nCenterIdx]->run(ec);
	//}
}

void WvsLogin::CenterAliveMonitor()
{
	printf("=================定期檢查Center Server連線程序=================\n");
	int centerSize = ConfigLoader::GetInstance()->IntValue("CenterCount");
	for (int i = 0; i < centerSize; ++i)
		if (aCenterList[i] && aCenterList[i]->IsConnectionFailed()) {
			aCenterList[i].reset();
			aCenterServerService[i]->stop();
			printf("Center Server %d 連線失敗，嘗試重新連線。\n", i);
			if (aCenterWorkThread[i]) 
			{
				aCenterWorkThread[i]->detach();
				*aCenterWorkThread[i] = std::thread(&WvsLogin::ConnectToCenter, this, i);
			}
			//
		}
}

void WvsLogin::InitializeCenter()
{
	int centerSize = ConfigLoader::GetInstance()->IntValue("CenterCount");
	for (int i = 0; i < centerSize; ++i)
		aCenterWorkThread[i].reset(new std::thread(&WvsLogin::ConnectToCenter, this, i));

	//Periodic Task, 檢測是否與Center中斷連線，並嘗試修正
	auto holderFunc = std::bind(&WvsLogin::CenterAliveMonitor, this);
	auto aliveHolder = AsnycScheduler::CreateTask(holderFunc, 3 * 100, true);
	aliveHolder->Start();
}

void WvsLogin::OnNotifySocketDisconnected(SocketBase *pSocket)
{
}