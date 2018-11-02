#include <cstdarg>
#include "WvsLogger.h"
#include "boost\lockfree\queue.hpp"
#include "..\Memory\MemoryPoolMan.hpp"
#include <thread>

//Win Only
#include <Windows.h>

std::mutex WvsLogger::m_mtxConsoleGuard;
std::condition_variable WvsLogger::m_cv;

boost::lockfree::queue<WvsLogger::WvsLogData*> g_qMsgQueue(WvsLogger::MAX_MSG_QUEUE_CAPACITY);
WvsLogger* WvsLogger::pInstnace = AllocObj( WvsLogger );

void WvsLogger::StartMonitoring()
{
	auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WvsLogData *pData;
	while (1)
	{
		std::unique_lock<std::mutex> lk(m_mtxConsoleGuard);
		if (g_qMsgQueue.empty())
			m_cv.wait(lk);
		if (g_qMsgQueue.pop(pData))
		{
			SetConsoleTextAttribute(hConsole, pData->m_nConsoleColor);
			printf("%s", pData->m_strData.c_str());
			FreeObj(pData);
			//delete pData;
			//SetConsoleTextAttribute(hConsole, WvsLogger::LEVEL_NORMAL);
		}
	}
}

WvsLogger::WvsLogger()
{
	setbuf(stdout, NULL);
	g_qMsgQueue.reserve(MAX_MSG_QUEUE_CAPACITY);
	std::thread *pThread = new std::thread(&WvsLogger::StartMonitoring, this);
}

WvsLogger::~WvsLogger()
{
	/*WvsLogData* pData = nullptr;
	while (g_qMsgQueue.pop(pData))
		delete pData;*/
}

void WvsLogger::PushLogImpl(int nConsoleColor, const std::string & strLog)
{
	WvsLogData* pLogData = AllocObj(WvsLogData);
	pLogData->m_nConsoleColor = nConsoleColor;
	pLogData->m_strData.assign(strLog);
	m_cv.notify_all();
	g_qMsgQueue.push(pLogData);
}

void WvsLogger::LogRaw(int nConsoleColor, const std::string & strLog)
{
	pInstnace->PushLogImpl(nConsoleColor, strLog);
}

void WvsLogger::LogRaw(const std::string & strLog)
{
	if(!pInstnace)
		pInstnace = AllocObj(WvsLogger);

	LogRaw(LEVEL_NORMAL, strLog);
}

void WvsLogger::LogFormat(const std::string format, ...)
{
	int final_n, n = ((int)format.size()) * 2, nOldSz = 0;
	char* formatted = nullptr;
	va_list ap;
	while (1)
	{
		if (formatted)
			FreeArray(formatted, nOldSz);
		nOldSz = n;
		formatted = AllocArray(char, n);
		va_start(ap, format);
		final_n = vsnprintf(&formatted[0], n, format.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}

	WvsLogData* pLogData = AllocObj(WvsLogData);
	pLogData->m_nConsoleColor = LEVEL_NORMAL;
	pLogData->m_strData = formatted;
	FreeArray(formatted, n);
	m_cv.notify_all();
	g_qMsgQueue.push(pLogData);
}

//Don't pass reference or pointer of "format"
void WvsLogger::LogFormat(int nConsoleColor, const std::string format, ...)
{
	int final_n, n = ((int)format.size()) * 2, nOldSz = 0;
	char* formatted = nullptr;
	va_list ap;
	while (1)
	{
		if (formatted)
			FreeArray(formatted, nOldSz);
		nOldSz = n;

		formatted = AllocArray(char, n);
		strcpy(&formatted[0], format.c_str());
		va_start(ap, format);
		final_n = vsnprintf(&formatted[0], n, format.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}

	WvsLogData* pLogData = AllocObj(WvsLogData);
	pLogData->m_nConsoleColor = nConsoleColor;
	pLogData->m_strData = formatted;
	FreeArray(formatted, n);
	m_cv.notify_all();
	g_qMsgQueue.push(pLogData);
}

WvsLogger::WvsLogData::WvsLogData()
{
	time_t rawtime;
	struct tm * timeinfo;
	char aBuffer[64];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(aBuffer, sizeof(aBuffer), "%d-%m-%Y %I:%M:%S", timeinfo);
	m_strDateTime.assign(aBuffer);
}
