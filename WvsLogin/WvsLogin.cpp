#include "WvsLogin.h"
#include <thread>
#include <iostream>
#include <functional>
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Task\AsnycScheduler.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Constants\ConfigLoader.hpp"
#include "..\WvsLib\Constants\ServerConstants.hpp"

WvsLogin::WvsLogin()
{
}


WvsLogin::~WvsLogin()
{
}

int WvsLogin::GetCenterCount() const
{
	return m_nCenterCount;
}

std::shared_ptr<Center>& WvsLogin::GetCenter(int idx)
{
	return m_apCenterInstance[idx];
}

void WvsLogin::ConnectToCenter(int nCenterIdx)
{
	m_apCenterInstance[nCenterIdx]->SetSocketDisconnectedCallBack(std::bind(&Center::OnNotifyCenterDisconnected, m_apCenterInstance[nCenterIdx].get()));
	m_apCenterInstance[nCenterIdx]->SetCenterIndex(nCenterIdx);
	m_apCenterInstance[nCenterIdx]->Connect(
		ConfigLoader::GetInstance()->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		ConfigLoader::GetInstance()->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);
}

void WvsLogin::CenterAliveMonitor(int nCenterIndex)
{
	if (m_apCenterInstance[nCenterIndex]->CheckSocketStatus(SocketBase::SocketStatus::eConnecting))
		return;
	WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "=================定期檢查Center Server連線程序=================\n");

	if (m_apCenterInstance[nCenterIndex] && (m_apCenterInstance[nCenterIndex]->CheckSocketStatus(SocketBase::SocketStatus::eClosed)))
	{
		WvsLogger::LogFormat("Center Server %d 連線失敗，嘗試重新連線。\n", nCenterIndex);

		m_apCenterWorkThread[nCenterIndex]->detach();
		*m_apCenterWorkThread[nCenterIndex] = (std::thread(&WvsLogin::ConnectToCenter, this, nCenterIndex));
	}
}

void WvsLogin::InitializeCenter()
{
	m_nCenterCount = ConfigLoader::GetInstance()->IntValue("CenterCount");
	for (int i = 0; i < m_nCenterCount; ++i)
	{
		aCenterServerService[i].reset(new asio::io_service());
		m_apCenterInstance[i] = std::make_shared<Center>(*aCenterServerService[i]);
		m_apCenterWorkThread[i].reset(new std::thread(&WvsLogin::ConnectToCenter, this, i));
		auto fHolderFunc = std::bind(&WvsLogin::CenterAliveMonitor, this, i);
		auto pAliveHolder = AsnycScheduler::CreateTask(fHolderFunc, 10 * 100, true);

		std::thread tCenterWorkThread([&] {
			asio::io_service::work work(*aCenterServerService[i]);
			std::error_code ec;
			aCenterServerService[i]->run(ec);
		});
		tCenterWorkThread.detach();
		pAliveHolder->Start();
	}
}

void WvsLogin::OnNotifySocketDisconnected(SocketBase *pSocket)
{
}