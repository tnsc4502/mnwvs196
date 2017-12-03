#pragma once
#include <atomic>
#include <mutex>
#include <map>

class Drop;
class User;
class Reward;
class Field;

class DropPool
{
	std::mutex m_mtxDropPoolLock;
	std::atomic<int> m_nDropIdCounter;
	std::map<int, Drop*> m_mDrop;
	bool m_bDropEverlasting = false;
	int m_tLastExpire;
	Field *m_pField;

public:

	void Create(Reward *reward, unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, unsigned int dwSourceID, int x1, int y1, int x2, int y2, int tDelay, int bAdmin, int nPos, bool bByPet);
	void OnEnter(User* pUser);
	DropPool(Field* pField);
	~DropPool();
};

