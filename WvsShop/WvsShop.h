#pragma once
#include "Center.h"
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"

class User;

class WvsShop : public WvsBase
{
	ConfigLoader* m_pCfgLoader;

	int m_nExternalPort = 0;
	int m_aExternalIP[4];

	std::shared_ptr<Center> m_pCenterInstance;
	asio::io_service* m_pCenterServerService;
	std::thread* m_pCenterWorkThread;

	std::mutex m_mUserLock;
	std::map<int, std::shared_ptr<User>> m_mUserMap;

	void ConnectToCenter();
	void CenterAliveMonitor();

public:
	WvsShop();
	~WvsShop();

	std::shared_ptr<Center>& GetCenter();
	void SetConfigLoader(ConfigLoader* pCfg);
	void InitializeCenter(); 

	User* FindUser(int nUserID);

	void OnUserConnected(std::shared_ptr<User> &pUser);
	void OnNotifySocketDisconnected(SocketBase *pSocket);
};

