#pragma once
#include "Net\WvsBase.h"
#include "Net\WorldInfo.h"

#include "Center.h"
#include <thread>
#include <map>
#include "User.h"

class WvsGame : public WvsBase
{
	std::mutex m_mUserLock;

	int nExternalPort = 0;
	int aExternalIP[4];
	std::map<int, std::shared_ptr<User>> mUserMap;

	std::shared_ptr<Center> aCenterPtr;
	asio::io_service* aCenterServerService;
	std::thread* aCenterWorkThread;

	void WvsGame::CenterAliveMonitor();

public:
	WvsGame();
	~WvsGame();

	std::shared_ptr<Center>& GetCenter()
	{
		return aCenterPtr;
	}

	//WvsGame 只允許一個Center
	int GetCenterCount() const { return 1; }

	void ConnectToCenter(int nCenterIdx);
	void WvsGame::InitializeCenter();

	void OnUserConnected(std::shared_ptr<User> &pUser);
	void OnNotifySocketDisconnected(SocketBase *pSocket);

	void SetExternalIP(const std::string& ip);
	void SetExternalPort(short nPort);

	int* GetExternalIP() const;
	short GetExternalPort() const;

	User* FindUser(int nUserID);
};