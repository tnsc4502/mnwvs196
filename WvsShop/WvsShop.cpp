#include "WvsShop.h"
#include <thread>
#include <iostream>
#include <functional>
#include "User.h"
#include "ClientSocket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"

WvsShop::WvsShop()
{
}


WvsShop::~WvsShop()
{
}

std::shared_ptr<Center>& WvsShop::GetCenter()
{
	return m_pCenterInstance;
}

void WvsShop::SetConfigLoader(ConfigLoader * pCfg)
{
	m_pCfgLoader = pCfg;
}

void WvsShop::ConnectToCenter()
{
	int nCenterIdx = 0;
	m_pCenterInstance->SetSocketDisconnectedCallBack(std::bind(&Center::OnNotifyCenterDisconnected, m_pCenterInstance.get()));
	m_pCenterInstance->SetCenterIndex(nCenterIdx);
	m_pCenterInstance->Connect(
		m_pCfgLoader->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		m_pCfgLoader->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);

}

void WvsShop::CenterAliveMonitor()
{
	if (m_pCenterInstance->CheckSocketStatus(SocketBase::SocketStatus::eConnecting))
		return;
	WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "=================定期檢查Center Server連線程序=================\n");
	int centerSize = m_pCfgLoader->IntValue("CenterCount");
	if (m_pCenterInstance && m_pCenterInstance->CheckSocketStatus(SocketBase::SocketStatus::eClosed))
	{
		WvsLogger::LogFormat("Center Server %d 連線失敗，嘗試重新連線。\n", 0);
		m_pCenterWorkThread->detach();
		*m_pCenterWorkThread = (std::thread(&WvsShop::ConnectToCenter, this));
	}
}

void WvsShop::InitializeCenter()
{
	m_pCenterServerService = (new asio::io_service());
	m_pCenterInstance = std::make_shared<Center>(*m_pCenterServerService);
	m_pCenterWorkThread = (new std::thread(&WvsShop::ConnectToCenter, this));
	auto fHolderFunc = std::bind(&WvsShop::CenterAliveMonitor, this);
	auto pAliveHolder = AsyncScheduler::CreateTask(fHolderFunc, 10 * 100, true);
	pAliveHolder->Start();


	std::thread tCenterWorkThread([&] {
		asio::io_service::work work(*m_pCenterServerService);
		std::error_code ec;
		m_pCenterServerService->run(ec);
	});
	tCenterWorkThread.detach();
}

User * WvsShop::FindUser(int nUserID)
{
	std::lock_guard<std::mutex> lockGuard(m_mUserLock);
	auto findIter = m_mUserMap.find(nUserID);
	return (findIter == m_mUserMap.end() ? nullptr : findIter->second.get());
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