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

void WvsShop::SetCenterOpened(bool bConnecting)
{
	aIsConnecting = bConnecting;
}

bool WvsShop::IsCenterOpened() const
{
	return aIsConnecting;
}

std::shared_ptr<Center>& WvsShop::GetCenter()
{
	return aCenterList;
}

void WvsShop::ConnectToCenter()
{
	try 
	{
		int nCenterIdx = 0;
		aCenterServerService = (new asio::io_service());
		aCenterList = std::make_shared<Center>(*aCenterServerService);
		aCenterList->SetDisconnectedNotifyFunc(&Center::OnNotifyCenterDisconnected);
		aCenterList->SetCenterIndex(nCenterIdx);
		WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "開啟Connection\n");
		aCenterList->OnConnectToCenter(
			ConfigLoader::GetInstance()->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
			ConfigLoader::GetInstance()->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
		);
		asio::io_service::work work(*aCenterServerService);
		std::error_code ec;
		aCenterServerService->run(ec);
	}
	catch (std::exception& e) 
	{
		printf("Exception occurred : %s\n", e.what());
	}
}

void WvsShop::CenterAliveMonitor()
{
	if (IsCenterOpened())
		return;
	SetCenterOpened(true);
	try 
	{
		WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "=================定期檢查Center Server連線程序=================\n");
		int centerSize = ConfigLoader::GetInstance()->IntValue("CenterCount");
		if (aCenterList && aCenterList->IsConnectionFailed())
		{
			aCenterList.reset();
			aCenterServerService->stop();
			WvsLogger::LogFormat("Center Server %d 連線失敗，嘗試重新連線。\n", 0);
			/*if (aCenterWorkThread[i])
			{
			aCenterWorkThread[i]->detach();
			*aCenterWorkThread[i] = std::thread(&WvsLogin::ConnectToCenter, this, i);
			}
			else*/
			aCenterWorkThread->detach();
			aCenterWorkThread = (new std::thread(&WvsShop::ConnectToCenter, this));
		}
	}
	catch (std::exception& e) 
	{
	}
}

void WvsShop::InitializeCenter()
{
	int centerSize = 1;
	for (int i = 0; i < centerSize; ++i)
	{
		SetCenterOpened(false);
		aCenterWorkThread = (new std::thread(&WvsShop::ConnectToCenter, this));
		auto holderFunc = std::bind(&WvsShop::CenterAliveMonitor, this);
		auto aliveHolder = AsnycScheduler::CreateTask(holderFunc, 10 * 100, true);
		aliveHolder->Start();
	}
}

void WvsShop::OnUserConnected(std::shared_ptr<User>& pUser)
{
	std::lock_guard<std::mutex> lockGuard(m_mUserLock);
	m_mUserMap[pUser->GetUserID()] = pUser;
}

void WvsShop::OnUserMigrateOut(SocketBase *pSocket)
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