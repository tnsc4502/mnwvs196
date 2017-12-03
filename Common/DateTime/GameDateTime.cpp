#include "GameDateTime.h"
#include <Windows.h>


GameDateTime::GameDateTime()
{
}


GameDateTime::~GameDateTime()
{
}

long long int GameDateTime::GetDateExpireFromPeriod(int nPeriod)
{
	_SYSTEMTIME st; // [sp+0h] [bp-18h]@1
	_ULARGE_INTEGER li; // [sp+10h] [bp-8h]@1

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, (LPFILETIME)&li);
	long long int ret = 0;
	int *date = (int*)&ret;
	date[0] = 600000000 * nPeriod + li.LowPart;
	return ret;
}

int GameDateTime::GetNowTime()
{
	return timeGetTime();
}
