#include "TimerThread.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Random\Rand32.h"
#include "Field.h"

std::vector<TimerThread*> TimerThread::m_aTimerPool;

TimerThread::TimerThread()
{
}


TimerThread::~TimerThread()
{
}

void TimerThread::Update()
{
	std::lock_guard<std::mutex> lock(m_mtxMutex);
	for (auto& pField : m_aFieldToUpdate)
		pField->Update();
}

void TimerThread::RegisterTimerPool(int nTimerCount, int nTick)
{
	//m_aTimerPool.resize(nTimerCount);
	TimerThread* pThreadTimer = nullptr;
	AsyncScheduler *pTimer = nullptr;
	for (int i = 0; i < nTimerCount; ++i)
	{
		pThreadTimer = AllocObj(TimerThread);
		pThreadTimer->m_aFieldToUpdate.clear();
		auto timerBind = std::bind(&(TimerThread::Update), pThreadTimer);
		pTimer = AsyncScheduler::CreateTask(timerBind, nTick, true);
		pThreadTimer->m_pTimer = pTimer;
		pTimer->Start();
		m_aTimerPool.push_back(pThreadTimer);
	}
}

void TimerThread::RegisterField(Field * pField)
{
	int nIdx = Rand32::GetInstance()->Random() % m_aTimerPool.size();
	m_aTimerPool[nIdx]->RegisterFieldImpl(pField);
}

void TimerThread::RegisterFieldImpl(Field * pField)
{
	std::lock_guard<std::mutex> lock(m_mtxMutex);
	m_aFieldToUpdate.push_back(pField);
}
