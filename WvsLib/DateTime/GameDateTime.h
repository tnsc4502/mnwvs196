#pragma once
class GameDateTime
{
public:
	GameDateTime();
	~GameDateTime();

	static long long int GetDateExpireFromPeriod(int nPeriod);
	static int GetTime();
};

