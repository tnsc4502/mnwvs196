#pragma once
#include <mutex>
#include <condition_variable>
#include "..\Common\CommonDef.h"

class WvsLogger
{
public:
	ALLOW_PRIVATE_ALLOC

	WvsLogger();
	~WvsLogger();

	static std::mutex m_mtxConsoleGuard; 
	static std::condition_variable m_cv;
	void StartMonitoring();

	static WvsLogger* pInstnace;

	void PushLogImpl(int nConsoleColor, const std::string& strLog);

	enum LOG_TEXT_COLOR 
	{
		TXT_BLACK = 0x00,
		TXT_BLUE = 0x01,
		TXT_GREEN = 0x02,
		TXT_AQUA = 0x03,
		TXT_RED = 0x04,
		TXT_PURPLE = 0x05,
		TXT_YELLOW = 0x06,
		TXT_WHITE = 0x07,
		TXT_GRAY = 0x08,

		TXT_LIGHT_BLUE = 9,
		TXT_LIGHT_GREEN = 0x0A,
		TXT_LIGHT_AQUQ = 0x0B,
		TXT_LIGHT_RED = 0x0C,
		TXT_LIGHT_PURPLE = 0x0D,
		TXT_LIGHT_YELLOW = 0x0E,
		TXT_LIGHT_WHITE = 0x0F,
	};

	enum LOG_BG_COLOR
	{
		BG_BLACK = 16 * 0x00,
		BG_BLUE = 16 * 0x01,
		BG_GREEN = 16 * 0x02,
		BG_AQUA = 16 * 0x03,
		BG_RED = 16 * 0x04,
		BG_PURPLE = 16 * 0x05,
		BG_YELLOW = 16 * 0x06,
		BG_WHITE = 16 * 0x07,
		BG_GRAY = 16 * 0x08,

		BG_LIGHT_BLUE = 16 * 0x09,
		BG_LIGHT_GREEN = 16 * 0x0A,
		BG_LIGHT_AQUQ = 16 * 0x0B,
		BG_LIGHT_RED = 16 * 0x0C,
		BG_LIGHT_PURPLE = 16 * 0x0D,
		BG_LIGHT_YELLOW = 16 * 0x0E,
		BG_LIGHT_WHITE = 16 * 0x0F,
	};

	enum LOG_COLOR_TYPE
	{
		LEVEL_NORMAL = TXT_LIGHT_WHITE | BG_BLACK,
		LEVEL_ERROR = TXT_LIGHT_RED ,
		LEVEL_WARNING = TXT_LIGHT_YELLOW ,
		LEVEL_INFO = TXT_LIGHT_AQUQ
	};

	static const int MAX_MSG_QUEUE_CAPACITY = 1000;

	struct WvsLogData
	{
		std::string m_strData, m_strDateTime;
		int m_nConsoleColor = LEVEL_NORMAL;

		WvsLogData();
	};

	static void LogRaw(const std::string& strLog);
	static void LogRaw(int nConsoleColor, const std::string& strLog);

	//Don't pass reference or pointer of "format"
	static void LogFormat(const std::string format, ...);
	static void LogFormat(int nConsoleColor, const std::string format, ...);
};

