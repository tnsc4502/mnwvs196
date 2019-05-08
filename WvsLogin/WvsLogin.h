#pragma once
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Common\WvsLoginConstants.hpp"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "Center.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class WvsLogin : public WvsBase
{
private:
	ConfigLoader* m_pCfgLoader;

	void ConnectToCenter(int nCenterIdx);

	int m_nCenterCount = 0;
	std::shared_ptr<Center> m_apCenterInstance[ServerConstants::kMaxNumberOfCenters];
	std::shared_ptr<asio::io_service> aCenterServerService[ServerConstants::kMaxNumberOfCenters];
	std::shared_ptr<std::thread> m_apCenterWorkThread[ServerConstants::kMaxNumberOfCenters];

	//紀錄Center instance是否正在連線，用於避免重連的異常
	bool aIsConnecting[ServerConstants::kMaxNumberOfCenters];
	void CenterAliveMonitor(int idx);

public:
	WvsLogin();
	~WvsLogin();

	int GetCenterCount() const;
	std::shared_ptr<Center>& GetCenter(int idx);
	void SetConfigLoader(ConfigLoader *pCfg);
	void InitializeCenter();
	void OnNotifySocketDisconnected(SocketBase *pSocket);
};

