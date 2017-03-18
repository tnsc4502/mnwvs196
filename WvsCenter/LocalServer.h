#pragma once
#include "Net\asio.hpp"
#include "Net\WvsBase.h"

class LocalServer : public WvsBase
{
private:

public:
	LocalServer();
	~LocalServer();

	/*static LocalServer* GetInstance()
	{
		static LocalServer *sWvsLogin = WvsBase::GetInstance<LocalServer>();
		return sWvsLogin;
	}*/
};

