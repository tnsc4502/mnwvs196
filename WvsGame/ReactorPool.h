#pragma once
#include <vector>
#include <map>
#include <string>
#include <atomic>
#include <mutex>

class Field;
class Reactor;
class User;
class InPacket;

class ReactorPool
{
public:
	struct ReactorGen
	{
		static std::atomic<int> stGenID;
		int nX = 0,
			nY = 0,
			tRegenInterval = 0,
			nTemplateID = 0,
			nReactorCount = 0,
			tRegenAfter = 0;
		bool bFlip = false, bForceGen = false;

		std::string sName;
	};

private:
	const static int UPDATE_INTERVAL = 7000;

	Field* m_pField;
	std::vector<ReactorGen> m_aReactorGen;
	std::map<int, Reactor*> m_mReactor;
	std::map<std::string, int> m_mReactorName;

	int m_tLastCreateReactorTime,
		m_nReactorTotalHit;

	std::mutex m_mtxReactorPoolMutex;
public:
	ReactorPool();
	~ReactorPool();

	void Init(Field* pField, void* pImg);
	void TryCreateReactor(bool bReset);
	void CreateReactor(ReactorGen *pPrg);
	void OnEnter(User *pUser);
	void OnPacket(User *pUser, int nType, InPacket *iPacket);
	void OnHit(User *pUser, InPacket *iPacket);
	void RemoveReactor(Reactor *pReactor);
	void Update(int tCur);
};

