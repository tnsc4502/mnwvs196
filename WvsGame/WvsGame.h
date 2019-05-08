#pragma once
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Net\WorldInfo.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"

#include "Center.h"
#include <thread>
#include <string>
#include <map>

class User;

class WvsGame : public WvsBase
{
	ConfigLoader* m_pCfgLoader;

	std::mutex m_mUserLock;
	std::map<int, std::shared_ptr<User>> m_mUserMap;
	std::string m_sCenterIP;
	std::shared_ptr<Center> m_pCenterInstance;
	asio::io_service* m_pCenterServerService;
	std::thread* m_pCenterWorkThread;
	int m_nChannelID = 0, m_nCenterPort = 0;

	void WvsGame::CenterAliveMonitor();

public:
	WvsGame();
	~WvsGame();

	std::shared_ptr<Center>& GetCenter();

	void ConnectToCenter(int nCenterIdx);

	void SetConfigLoader(ConfigLoader* pCfg);
	void WvsGame::InitializeCenter();

	void OnUserConnected(std::shared_ptr<User> &pUser);
	void OnNotifySocketDisconnected(SocketBase *pSocket);


	int GetChannelID() const;

	User* FindUser(int nUserID);
};