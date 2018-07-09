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

	int m_nExternalPort = 0, m_nChannelID = 0;
	int m_aExternalIP[4];
	bool m_bIsCenterConnecting = false;

	std::map<int, std::shared_ptr<User>> m_mUserMap;

	std::shared_ptr<Center> m_pCenterPtr;
	asio::io_service* m_pCenterServerService;
	std::thread* m_CenterWorkThread;

	void WvsGame::CenterAliveMonitor();

public:
	WvsGame();
	~WvsGame();

	std::shared_ptr<Center>& GetCenter()
	{
		return m_pCenterPtr;
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
	int GetChannelID() const;

	bool IsCenterConnecting() const;
	void SetCenterConnecting(bool bConnecting);

	User* FindUser(int nUserID);
};