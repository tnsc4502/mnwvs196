#pragma once
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Net\WorldInfo.h"

#include "Center.h"
#include <thread>
#include <map>
#include "User.h"

class WvsGame : public WvsBase
{
	std::mutex m_mUserLock;
	std::map<int, std::shared_ptr<User>> m_mUserMap;
	std::shared_ptr<Center> m_pCenterInstance;
	asio::io_service* m_pCenterServerService;
	std::thread* m_pCenterWorkThread;
	int m_nChannelID = 0;

	void WvsGame::CenterAliveMonitor();

public:
	WvsGame();
	~WvsGame();

	std::shared_ptr<Center>& GetCenter();

	void ConnectToCenter(int nCenterIdx);
	void WvsGame::InitializeCenter();

	void OnUserConnected(std::shared_ptr<User> &pUser);
	void OnNotifySocketDisconnected(SocketBase *pSocket);


	int GetChannelID() const;

	User* FindUser(int nUserID);
};