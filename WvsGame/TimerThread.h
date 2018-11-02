#pragma once
#include <vector>
#include <mutex>
#include "..\WvsLib\Common\CommonDef.h"

class Field;
class AsyncScheduler;

class TimerThread
{
	ALLOW_PRIVATE_ALLOC 

	static std::vector<TimerThread*> m_aTimerPool;

	AsyncScheduler* m_pTimer;
	std::vector<Field*> m_aFieldToUpdate;
	std::mutex m_mtxMutex;

	TimerThread();
	~TimerThread();

	void Update();
	void RegisterFieldImpl(Field *pField);
public:

	static void RegisterTimerPool(int nTimerCount, int nTick);
	static void RegisterField(Field *pField);
};

