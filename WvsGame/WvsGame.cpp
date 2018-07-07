#include "WvsGame.h"
#include "..\WvsLib\Constants\WvsGameConstants.hpp"
#include "..\WvsLib\Task\AsnycScheduler.h"
#include "..\WvsLib\Constants\ConfigLoader.hpp"
#include "ClientSocket.h"
#include "..\WvsLib\Logger\WvsLogger.h"

WvsGame::WvsGame()
{

}

WvsGame::~WvsGame()
{

}

void WvsGame::ConnectToCenter(int nCenterIdx)
{
	m_pCenterServerService = new asio::io_service();
	m_pCenterPtr = std::make_shared<Center>(*m_pCenterServerService);
	m_pCenterPtr->SetDisconnectedNotifyFunc(&Center::OnNotifyCenterDisconnected);
	m_pCenterPtr->SetCenterIndex(nCenterIdx);
	m_pCenterPtr->OnConnectToCenter(
		ConfigLoader::GetInstance()->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		ConfigLoader::GetInstance()->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);
	asio::io_service::work work(*m_pCenterServerService);
	std::error_code ec;
	m_pCenterServerService->run(ec);
}

void WvsGame::CenterAliveMonitor()
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "=================定期檢查Center Server連線程序=================\n");
	if (m_pCenterPtr && m_pCenterPtr->GetWorldInfo().bConnectionFailed) 
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Center Server %d 尚未連線，嘗試重新連線。\n", 0);
		m_pCenterPtr.reset();
		m_pCenterServerService->stop();
		m_CenterWorkThread->detach();
		m_CenterWorkThread = new std::thread(&WvsGame::ConnectToCenter, this, 0);
		new std::thread(&WvsGame::ConnectToCenter, this, 0);
	}
}

void WvsGame::InitializeCenter()
{
	m_CenterWorkThread = new std::thread(&WvsGame::ConnectToCenter, this, 0);
	auto holderFunc = std::bind(&WvsGame::CenterAliveMonitor, this);
	auto aliveHolder = AsnycScheduler::CreateTask(holderFunc, 10 * 1000, true);
	aliveHolder->Start();
	m_nChannelID = ConfigLoader::GetInstance()->IntValue("ChannelID");
	//ConnectToCenter(0);
	//WvsGameConstants::CenterServerList[0].nServerPort = ConfigLoader::GetInstance()->IntValue("Center0_Port");
	//WvsGameConstants::CenterServerList[0].strServerIP = ConfigLoader::GetInstance()->StrValue("Center0_IP");
	//auto& aCenterInfoList = WvsGameConstants::CenterServerList;
	//int centerSize = sizeof(aCenterInfoList) / sizeof(aCenterInfoList[0]);
	//for (int i = 0; i < centerSize; ++i)
	////aCenterWorkThread = new std::thread(&WvsGame::ConnectToCenter, this, 0);
	//auto holderFunc = std::bind(&WvsGame::CenterAliveMonitor, this);
	//auto aliveHolder = AsnycScheduler::CreateTask(holderFunc, 10 * 1000, true);
	//aliveHolder->Start();
}

void WvsGame::OnUserConnected(std::shared_ptr<User> &pUser)
{
	std::lock_guard<std::mutex> lockGuard(m_mUserLock);
	m_mUserMap[pUser->GetUserID()] = pUser;
}

void WvsGame::OnNotifySocketDisconnected(SocketBase *pSocket)
{
	std::lock_guard<std::mutex> lockGuard(m_mUserLock);
	auto pClient = (ClientSocket*)pSocket;
	if (pClient->GetUser())
	{
		m_mUserMap.erase(pClient->GetUser()->GetUserID());
		pClient->SetUser(nullptr);
	}
}

void WvsGame::SetExternalIP(const std::string& ip)
{
	int ipPos = 0, strPos = 0;
	std::string tmpStr = "";
	while (strPos <= ip.size() )
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

void WvsGame::SetExternalPort(short nPort)
{
	m_nExternalPort = nPort;
}

int* WvsGame::GetExternalIP() const
{
	return (int*)m_aExternalIP;
}

short WvsGame::GetExternalPort() const
{
	return m_nExternalPort;
}

int WvsGame::GetChannelID() const
{
	return m_nChannelID;
}

User * WvsGame::FindUser(int nUserID)
{
	std::lock_guard<std::mutex> lockGuard(m_mUserLock);
	auto findIter = m_mUserMap.find(nUserID);
	if (findIter == m_mUserMap.end())
		return nullptr;
	return findIter->second.get();
}
