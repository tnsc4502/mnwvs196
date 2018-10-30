#include "GameDateTime.h"
#include <Windows.h>
#include <chrono>

GameDateTime::GameDateTime()
{
}


GameDateTime::~GameDateTime()
{
}

long long int GameDateTime::GetCurrentDate()
{
	return GetDateExpireFromPeriodAsSec(0);
}

long long int GameDateTime::GetDateExpireFromPeriod(int nPeriod)
{
	return GetDateExpireFromPeriodAsSec(nPeriod * 24 * 60 * 60);
}

long long int GameDateTime::GetDateExpireFromPeriodAsSec(int nSec)
{
	namespace sc = std::chrono;
	auto time = sc::system_clock::now();
	auto since_epoch = time.time_since_epoch();
	auto millis = sc::duration_cast<sc::milliseconds>(since_epoch);
	return (millis.count() + nSec * 1000) * 10000 + 116444592000000000L;
}

int GameDateTime::GetTime()
{
	return timeGetTime();
}
