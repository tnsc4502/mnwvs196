#pragma once
#include "Center.h"
#include "..\WvsLib\Net\WvsBase.h"
class User;

class WvsShop : public WvsBase
{
	int m_nExternalPort = 0;
	int m_aExternalIP[4];

	std::shared_ptr<Center> aCenterList;
	asio::io_service* aCenterServerService;
	std::thread* aCenterWorkThread;

	std::mutex m_mUserLock;
	std::map<int, std::shared_ptr<User>> m_mUserMap;

	//紀錄Center instance是否正在連線，用於避免重連的異常
	bool aIsConnecting;

	void ConnectToCenter();
	void CenterAliveMonitor();

public:
	WvsShop();
	~WvsShop();

	void SetExternalIP(const std::string& ip);
	void SetExternalPort(short nPort);
	int* GetExternalIP() const;
	short GetExternalPort() const;

	std::shared_ptr<Center>& GetCenter();
	void InitializeCenter(); 

	void OnUserConnected(std::shared_ptr<User> &pUser);
	void OnUserMigrateOut(SocketBase *pSocket);
	void OnNotifySocketDisconnected(SocketBase *pSocket) {}

	//設定Center instance的連線狀況
	void SetCenterOpened(bool bConnecting);

	//取得Center instance的連線狀況
	bool IsCenterOpened() const;
};

