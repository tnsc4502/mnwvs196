#pragma once
#include <agents.h>
#include <ppltasks.h>
#include <functional>

namespace AsnycScheduler
{
	template<typename FUNC_TYPE>
	class AsnycScheduler
	{
	private:
		FUNC_TYPE mFunc; //bind函式

		unsigned int mTimePeriod; //每幾ms執行一次?

		bool mIsRepeat; //重複執行 ?
		bool mIsStarted;

		Concurrency::timer<int> *mTimerInstance = nullptr;

		static void DelayCall(AsnycScheduler<FUNC_TYPE> * __instancePtr)
		{
			concurrency::task_completion_event<void> tce;
			auto call = new concurrency::call<int>(
				[__instancePtr, tce](int)
			{
				__instancePtr->mFunc();
				tce.set();
			});

			__instancePtr->mTimerInstance = new concurrency::timer<int>(__instancePtr->mTimePeriod, 0, call, __instancePtr->mIsRepeat);
			concurrency::task<void> event_set(tce);
			event_set.then([__instancePtr, call]()
			{
				if (!__instancePtr->mIsRepeat)
				{
					delete __instancePtr->mTimerInstance;
					delete call;
				}
			});

			__instancePtr->mTimerInstance->start();
		}

	public:
		AsnycScheduler(FUNC_TYPE function, unsigned int timeInMs, bool repeat) :
			mFunc(function),
			mTimePeriod(timeInMs),
			mIsRepeat(repeat),
			mIsStarted(false)
		{}

		~AsnycScheduler() {}

		void Start()
		{
			mIsStarted = true;
			DelayCall(this);
		}

		void Abort()
		{
			if (mTimerInstance)
			{
				mTimerInstance->stop();
				delete mTimerInstance;
				mTimerInstance = nullptr;
				mIsStarted = false;
			}
		}

		bool IsStarted() const
		{
			return mIsStarted;
		}
	};

	template<typename FUNC_TYPE>
	static AsnycScheduler<FUNC_TYPE>* CreateTask(FUNC_TYPE function, unsigned int timeInMs, bool repeat)
	{
		return new AsnycScheduler<FUNC_TYPE>(function, timeInMs, repeat);
	}
};
