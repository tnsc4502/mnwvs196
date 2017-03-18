#pragma once
#include "Net\asio.hpp"

class WvsWorld
{
private:
	asio::io_service mIOService;

public:
	WvsWorld();
	~WvsWorld();

	static WvsWorld *GetInstance()
	{
		static WvsWorld *sWvsWorld = new WvsWorld();

		return sWvsWorld;
	}

	void Init();
	asio::io_service& GetIOService();
};

