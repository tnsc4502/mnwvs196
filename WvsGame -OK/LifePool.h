#pragma once
#include <map>
#include "Npc.h"
#include "Mob.h"
#include "..\Common\Wz\WzResMan.hpp"
#include <atomic>
#include <mutex>

class User;
class Field;
class Controller;

class LifePool
{
	const static int MAX_WINDOW_VIEW_X = 1280, MAX_WINDOW_VIEW_Y = 1024, MAX_MOB_GEN = 40;

	std::mutex m_lifePoolMutex;

	std::atomic<int> atomicObjectCounter = 0x1000;

	//在這個地圖中所有可能的NPC物件
	std::vector<Npc> m_lNpc;

	//在這個地圖中所有可能的怪物物件
	std::vector<Mob> m_lMob;

	//在這個地圖中真正產生出的怪物, key是Object ID
	std::map<int, Mob*> m_aMobGen;

	//在這個地圖中真正產生出的Npc, key是Object ID
	std::map<int, Npc*> m_aNpcGen;

	/* 地圖中有關玩家控制權的紀錄，使用std::map 不須像正服創建 Max Heap 與 Min Heap
	   Key是 控制數量 Value : Controller是代表控制者的實體指標
	   User必須保存map的iterator 以便進行快速刪除 [ std::map的iterator不會因為容器本身新增刪除而無效 ]
	*/
	std::map<int, Controller*> m_hCtrl;

	//紀錄User對應到的m_hCtrl 
	std::map<User*, decltype(m_hCtrl)::iterator> m_mController;

	void SetFieldObjAttribute(FieldObj* pFieldObj, WZ::Node& dataNode);

	int m_nMobCapacityMin, m_nMobCapacityMax, m_aInitMobGenCount = 0, m_nSubMobCount = -1, m_nMobDamagedByMobState = 0;

	bool m_bMobGenEnable = true;

	Field* m_pField;

public:
	LifePool();
	~LifePool();

	void SetMaxMobCapacity(int max)
	{
		m_nMobCapacityMax = max;
	}

	int GetMaxMobCapacity() const
	{
		return m_nMobCapacityMax;
	}

	//從Wz中讀取此地圖中Npc資訊，存到m_lNpc中
	void LoadNpcData(WZ::Node& dataNode);

	//從Wz中讀取此地圖中Mob資訊，存到m_lMob中
	void LoadMobData(WZ::Node& dataNode);

	void CreateNpc(const Npc&);

	void TryCreateMob(bool reset);
	void CreateMob(const Mob&);
	
	/*void PrintNpc()
	{
		for (auto& x : mNpcMap)
			std::cout << x.second.GetTemplateID() << std::endl;
	}*/

	void Init(Field* pField, int nFieldID);
	void OnEnter(User* pUser);

	void InsertController(User* pUser);
	void RemoveController(User* pUser);
};

