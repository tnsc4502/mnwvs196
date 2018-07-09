#include "WvsShop.h"
#include <thread>
#include <iostream>
#include <functional>
#include "User.h"
#include "ClientSocket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Task\AsnycScheduler.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Constants\ConfigLoader.hpp"
#include "..\WvsLib\Constants\ServerConstants.hpp"

WvsShop::WvsShop()
{
}


WvsShop::~WvsShop()
{
}

void WvsShop::SetCenterConnecting(bool bConnecting)
{
	aIsConnecting = bConnecting;
}

bool WvsShop::IsCenterConnecting() const
{
	return aIsConnecting;
}

std::shared_ptr<Center>& WvsShop::GetCenter()
{
	return aCenterList;
}

void WvsShop::ConnectToCenter()
{
	int nCenterIdx = 0;
	aCenterList->SetDisconnectedNotifyFunc(&Center::OnNotifyCenterDisconnected);
	aCenterList->SetCenterIndex(nCenterIdx);
	aCenterList->OnConnectToCenter(
		ConfigLoader::GetInstance()->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		ConfigLoader::GetInstance()->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);

}

void WvsShop::CenterAliveMonitor()
{
	if (IsCenterConnecting())
		return;
	SetCenterConnecting(true);
	WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "=================定期檢查Center Server連線程序=================\n");
	int centerSize = ConfigLoader::GetInstance()->IntValue("CenterCount");
	if (aCenterList && aCenterList->IsConnectionFailed())
	{
		WvsLogger::LogFormat("Center Server %d 連線失敗，嘗試重新連線。\n", 0);
		aCenterWorkThread->detach();
		*aCenterWorkThread = (std::thread(&WvsShop::ConnectToCenter, this));
	}
}

void WvsShop::InitializeCenter()
{
	int centerSize = 1;
	for (int i = 0; i < centerSize; ++i)
	{
		m_pCenterServerService = (new asio::io_service());
		aCenterList = std::make_shared<Center>(*m_pCenterServerService);
		SetCenterConnecting(false);
		aCenterWorkThread = (new std::thread(&WvsShop::ConnectToCenter, this));
		auto holderFunc = std::bind(&WvsShop::CenterAliveMonitor, this);
		auto aliveHolder = AsnycScheduler::CreateTask(holderFunc, 10 * 100, true);
		aliveHolder->Start();
	}

	std::thread tCenterWorkThread([&] {
		asio::io_service::work work(*m_pCenterServerService);
		std::error_code ec;
		m_pCenterServerService->run(ec);
	});
	tCenterWorkThread.detach();
}

void WvsShop::OnUserConnected(std::shared_ptr<User>& pUser)
{
	std::lock_guard<std::mutex> lockGuard(m_mUserLock);
	m_mUserMap[pUser->GetUserID()] = pUser;
}

void WvsShop::OnNotifySocketDisconnected(SocketBase * pSocket)
{
	std::lock_guard<std::mutex> lockGuard(m_mUserLock);
	auto pClient = (ClientSocket*)pSocket;
	if (pClient->GetUser())
	{
		m_mUserMap.erase(pClient->GetUser()->GetUserID());
		pClient->SetUser(nullptr);
	}
}

int* WvsShop::GetExternalIP() const
{
	return (int*)m_aExternalIP;
}

short WvsShop::GetExternalPort() const
{
	return m_nExternalPort;
}

void WvsShop::SetExternalIP(const std::string& ip)
{
	int ipPos = 0, strPos = 0;
	std::string tmpStr = "";
	while (strPos <= ip.size())
	{
		if (strPos == ip.size() || ip[strPos] == '.')
		{
			m_aExternalIP[ipPos++] = atoi(tmpStr.c_str());
			tmpStr = "";
		}
		else
			tmpStr += ip[strPos];
		++strPos;
	}
}

void WvsShop::SetExternalPort(short nPort)
{
	m_nExternalPort = nPort;
}