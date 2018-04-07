#include <cstdarg>
#include "WvsLogger.h"
#include "boost\lockfree\queue.hpp"
#include <thread>

//Win Only
#include <Windows.h>

boost::lockfree::queue<WvsLogger::WvsLogData*> g_qMsgQueue(WvsLogger::MAX_MSG_QUEUE_CAPACITY);
WvsLogger* WvsLogger::pInstnace = new WvsLogger;

void WvsLogger::StartMonitoring()
{
	auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WvsLogData *pData;
	while (1)
	{
		while (g_qMsgQueue.pop(pData))
		{
			SetConsoleTextAttribute(hConsole, pData->m_nConsoleColor);
			printf("%s", pData->m_strData.c_str());
			delete pData;
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
	WvsLogData* pLogData = new WvsLogData;
	pLogData->m_nConsoleColor = nConsoleColor;
	pLogData->m_strData.assign(strLog);
	g_qMsgQueue.push(pLogData);
}

void WvsLogger::LogRaw(int nConsoleColor, const std::string & strLog)
{
	pInstnace->PushLogImpl(nConsoleColor, strLog);
}

void WvsLogger::LogRaw(const std::string & strLog)
{
	LogRaw(LEVEL_NORMAL, strLog);
}

void WvsLogger::LogFormat(const std::string format, ...)
{
	int final_n, n = ((int)format.size()) * 2;
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1)
	{
		formatted.reset(new char[n]);
		strcpy(&formatted[0], format.c_str());
		va_start(ap, format);
		final_n = vsnprintf(&formatted[0], n, format.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}

	WvsLogData* pLogData = new WvsLogData;
	pLogData->m_nConsoleColor = LEVEL_NORMAL;
	pLogData->m_strData = std::move(formatted.get());
	g_qMsgQueue.push(pLogData);
}

//Don't pass reference or pointer of "format"
void WvsLogger::LogFormat(int nConsoleColor, const std::string format, ...)
{
	int final_n, n = ((int)format.size()) * 2;
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1)
	{
		formatted.reset(new char[n]);
		strcpy(&formatted[0], format.c_str());
		va_start(ap, format);
		final_n = vsnprintf(&formatted[0], n, format.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}

	WvsLogData* pLogData = new WvsLogData;
	pLogData->m_nConsoleColor = nConsoleColor;
	pLogData->m_strData = std::move(formatted.get());
	g_qMsgQueue.push(pLogData);
}

WvsLogger::WvsLogData::WvsLogData()
{
	time_t rawtime;
	struct tm * timeinfo;
	char aBuffer[60];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(aBuffer, sizeof(aBuffer), "%d-%m-%Y %I:%M:%S", timeinfo);
	m_strDateTime.assign(aBuffer);
}
