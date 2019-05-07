#pragma once
#include <agents.h>
#include <ppltasks.h>
#include <functional>
#include "..\Memory\MemoryPoolMan.hpp"

class AsyncScheduler
{
public:
	unsigned int mTimePeriod; //每幾ms執行一次?
	bool mIsRepeat, mIsStarted, m_bTaskDone = false;

	Concurrency::timer<int> *mTimerInstance = nullptr;
	Concurrency::call<int> *mCall = nullptr;

public:
	template<typename FUNC_TYPE>
	friend static AsyncScheduler* CreateTask2(FUNC_TYPE function, unsigned int timeInMs, bool repeat);

	AsyncScheduler(unsigned int timeInMs, bool repeat) :
		mTimePeriod(timeInMs),
		mIsRepeat(repeat),
		mIsStarted(false)
	{}

	~AsyncScheduler()
	{
		FreeObj( mTimerInstance );
		FreeObj( mCall );
	}

	void Start()
	{
		if (!mIsRepeat && m_bTaskDone)
		{
			mTimerInstance = AllocObjCtor(concurrency::timer<int>)(
				mTimePeriod,
				0,
				mCall,
				mIsRepeat);
		}
		m_bTaskDone = false;
		mIsStarted = true;
		mTimerInstance->start();
	}

	void Pause()
	{
		m_bTaskDone = true;
		mIsStarted = false;
		mTimerInstance->pause();
	}

	void Abort()
	{
		if (mTimerInstance)
		{
			mTimerInstance->stop();
			mIsStarted = false;
		}
	}

	bool IsStarted() const
	{
		return mIsStarted;
	}

	bool IsTaskDone() const
	{
		return m_bTaskDone;
	}

	void OnTick()
	{
		m_bTaskDone = true;
		if (!mIsRepeat)
		{
			mTimerInstance->stop();
			FreeObj( mTimerInstance );
		}
	}

	template<typename FUNC_TYPE>
	static AsyncScheduler* CreateTask(FUNC_TYPE function, unsigned int timeInMs, bool repeat)
	{
		auto __instancePtr = AllocObjCtor(AsyncScheduler)(timeInMs, repeat);

		FUNC_TYPE func = function;
		auto call = AllocObjCtor(concurrency::call<int>)(
			[&, __instancePtr, func](int)
		{
			func();
			__instancePtr->OnTick();
		});
		__instancePtr->mCall = call;

		__instancePtr->mTimerInstance = AllocObjCtor(concurrency::timer<int>)(
			__instancePtr->mTimePeriod,
			0,
			call,
			__instancePtr->mIsRepeat);
		return __instancePtr;
	}
};
