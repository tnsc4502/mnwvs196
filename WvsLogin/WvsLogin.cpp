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

void WvsLogin::SetCenterConnecting(int nCenterIdx, bool bConnecting)
{ 
	aIsConnecting[nCenterIdx] = bConnecting;
}

bool WvsLogin::IsCenterConnecting(int nCenterIdx) const
{
	return aIsConnecting[nCenterIdx];
}

std::shared_ptr<Center>& WvsLogin::GetCenter(int idx)
{
	return aCenterList[idx];
}

void WvsLogin::ConnectToCenter(int nCenterIdx)
{
	aCenterList[nCenterIdx]->SetDisconnectedNotifyFunc(&Center::OnNotifyCenterDisconnected);
	aCenterList[nCenterIdx]->SetCenterIndex(nCenterIdx);
	WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "開啟Connection\n");
	aCenterList[nCenterIdx]->OnConnectToCenter(
		ConfigLoader::GetInstance()->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		ConfigLoader::GetInstance()->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);
}

void WvsLogin::CenterAliveMonitor(int i)
{
	if (IsCenterConnecting(i))
		return;
	SetCenterConnecting(i, true);
	WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "=================定期檢查Center Server連線程序=================\n");
	int centerSize = ConfigLoader::GetInstance()->IntValue("CenterCount");
	if (aCenterList[i] && aCenterList[i]->IsConnectionFailed())
	{
		WvsLogger::LogFormat("Center Server %d 連線失敗，嘗試重新連線。\n", i);

		aCenterWorkThread[i]->detach();
		*aCenterWorkThread[i] = (std::thread(&WvsLogin::ConnectToCenter, this, i));
	}

}

void WvsLogin::InitializeCenter()
{
	m_nCenterCount = ConfigLoader::GetInstance()->IntValue("CenterCount");
	for (int i = 0; i < m_nCenterCount; ++i)
	{
		SetCenterConnecting(i, false);
		aCenterServerService[i].reset(new asio::io_service());
		aCenterList[i] = std::make_shared<Center>(*aCenterServerService[i]);
		aCenterWorkThread[i].reset(new std::thread(&WvsLogin::ConnectToCenter, this, i));
		auto holderFunc = std::bind(&WvsLogin::CenterAliveMonitor, this, i);
		auto aliveHolder = AsnycScheduler::CreateTask(holderFunc, 10 * 100, true);

		std::thread tCenterWorkThread([&] {
			asio::io_service::work work(*aCenterServerService[i]);
			std::error_code ec;
			aCenterServerService[i]->run(ec);
		});
		tCenterWorkThread.detach();
		aliveHolder->Start();
	}
}

void WvsLogin::OnNotifySocketDisconnected(SocketBase *pSocket)
{
}