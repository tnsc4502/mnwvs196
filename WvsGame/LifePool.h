#pragma once
#include <map>
#include "Npc.h"
#include "Mob.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include <atomic>
#include <mutex>

struct AttackInfo;
class User;
class Field;
class Controller;
class InPacket;
class SkillEntry;
class Drop;

class LifePool
{
	const static int MAX_WINDOW_VIEW_X = 1280, MAX_WINDOW_VIEW_Y = 1024, MAX_MOB_GEN = 40;

	std::mutex m_lifePoolMutex;

	std::atomic<int> atomicObjectCounter = 0x1000, atomicForceAtomCounter = 0x1000;

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
	std::multimap<int, Controller*> m_hCtrl;

	/*
	當地圖中無任何玩家可作為Controller，將所有怪物控制權轉交給Null Controller，方便管理
	*/
	Controller* m_pCtrlNull;

	//紀錄User對應到的m_hCtrl iterator
	std::map<int, decltype(m_hCtrl)::iterator> m_mController;

	//設定物件在地圖中的基本屬性
	void SetFieldObjAttribute(FieldObj* pFieldObj, WZ::Node& dataNode);

	//場域有關的屬性
	int m_nMobCapacityMin, m_nMobCapacityMax, m_aInitMobGenCount = 0, m_nSubMobCount = -1, m_nMobDamagedByMobState = 0;

	bool m_bMobGenEnable = true;

	Field* m_pField;

	void OnMobPacket(User* pUser, int nType, InPacket* iPacket);
	void OnNpcPacket(User* pUser, int nType, InPacket* iPacket);
public:
	LifePool();
	~LifePool();

	void SetMaxMobCapacity(int max);
	int GetMaxMobCapacity() const;

	//從Wz中讀取此地圖中Npc資訊，存到m_lNpc中
	void LoadNpcData(WZ::Node& dataNode);

	//從Wz中讀取此地圖中Mob資訊，存到m_lMob中
	void LoadMobData(WZ::Node& dataNode);

	/*
	產生出Npc實體
	*/
	void CreateNpc(const Npc&);

	/*
	若地圖中的怪物數量不足則隨機挑選lMob並CreateMob
	*/
	void TryCreateMob(bool reset);

	/*
	將指定的Mob召喚出並加入aMobGen
	*/
	void CreateMob(const Mob&, int x, int y, int fh, int bNoDropPriority, int nType, unsigned int dwOption, int bLeft, int nMobType, Controller* pOwner);
	void RemoveMob(Mob* pMob);

	void Init(Field* pField, int nFieldID);
	void OnEnter(User* pUser);

	/*
	有玩家進入地圖，插入Controller並重新分配怪物控制權[RedistributeLife]
	*/
	void InsertController(User* pUser);

	/*
	玩家離開地圖，將該玩家控制的怪物分配給minCtrl或nullCtrl
	*/
	void RemoveController(User* pUser);

	/*
	Controller異動時，從新插入hCtrl中
	*/
	void UpdateCtrlHeap(Controller* pController);

	/*
	還沒寫, 作用不明
	*/
	bool GiveUpMobController(Controller* pController);

	/*
	重新分配地圖中的怪物控制權
	先將nullCtrl的怪物分配給minCtrl 接著平衡maxCtrl與minCtrl的控制數量
	*/
	void RedistributeLife();

	void Update();

	void OnPacket(User* pUser, int nType, InPacket* iPacket);
	void OnUserAttack(User *pUser, const SkillEntry *pSkill, AttackInfo *pInfo);
	void EncodeAttackInfo(User * pUser, AttackInfo *pInfo, OutPacket *oPacket);

	std::mutex& GetLock();

	Npc* GetNpc(int nFieldObjID);
	Mob* GetMob(int nFieldObjID);

	int GetForceAtomObjectID();
};

