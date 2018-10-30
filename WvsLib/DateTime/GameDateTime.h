#pragma once
class GameDateTime
{
public:
	GameDateTime();
	~GameDateTime();

	const static long long int TIME_PERMANENT = 150841440000000000L;
	const static long long int TIME_UNLIMITED = 94354848000000000L;

	static long long int GetCurrentDate();
	static long long int GetDateExpireFromPeriod(int nPeriod);
	static long long int GetDateExpireFromPeriodAsSec(int nSec);
	static int GetTime();
};

