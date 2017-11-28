#include "WvsLogin.h"
#include <thread>
#include <iostream>
#include "Net\OutPacket.h"

#include "Constants\ConfigLoader.hpp"

WvsLogin::WvsLogin()
{
}


WvsLogin::~WvsLogin()
{
}

void WvsLogin::ConnectToCenter(int nCenterIdx)
{
	aCenterServerService[nCenterIdx] = new asio::io_service();		
	aCenterList[nCenterIdx] = std::make_shared<Center>(*aCenterServerService[nCenterIdx]);
	aCenterList[nCenterIdx]->SetDisconnectedNotifyFunc(OnSocketDisconnected);
	aCenterList[nCenterIdx]->SetCenterIndex(nCenterIdx);
	aCenterList[nCenterIdx]->OnConnectToCenter(
		ConfigLoader::GetInstance()->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		ConfigLoader::GetInstance()->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);
	asio::io_service::work work(*aCenterServerService[nCenterIdx]);
	std::error_code ec;
	aCenterServerService[nCenterIdx]->run(ec);
}

void WvsLogin::InitializeCenter()
{
	int centerSize = ConfigLoader::GetInstance()->IntValue("CenterCount");
	for (int i = 0; i < centerSize; ++i)
		aCenterWorkThread[i] = new std::thread(&WvsLogin::ConnectToCenter, this, i);
}

void WvsLogin::OnNotifySocketDisconnected(SocketBase *pSocket)
{

}