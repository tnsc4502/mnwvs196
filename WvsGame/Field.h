#pragma once
#include <map>
#include <mutex>
#include <functional>
#include "FieldPoint.h"

class LifePool;
class Mob;
class Portal;
class PortalMap;
class TownPortalPool;
class ReactorPool;
class DropPool;
class FieldSet;
class User;
class AsyncScheduler;
class InPacket;
class OutPacket;
class WvsPhysicalSpace2D;
class SummonedPool;

class Field
{
	std::mutex m_mtxFieldUserMutex, m_mtxFieldLock;
	std::map<int, User*> m_mUser; //m_lUser in WvsGame.idb
	int m_nFieldID = 0;
	LifePool *m_pLifePool;
	PortalMap *m_pPortalMap;
	DropPool *m_pDropPool;
	TownPortalPool *m_pTownPortalPool;
	ReactorPool *m_pReactorPool;
	SummonedPool* m_pSummonedPool;
	FieldSet* m_pParentFieldSet = nullptr;
	WvsPhysicalSpace2D* m_pSpace2D;
	FieldPoint m_ptLeftTop, m_szMap;

	std::string m_sStreetName, 
				m_sMapName, 
				m_sWeatherMsg; //商城心情道具訊息?

	int m_nWeatherItemID, //商城心情道具物品ID?
		m_nJukeBoxItemID; //Juke Box?

	double m_dIncRate_EXP = 1.0, 
		   m_dIncRate_Drop = 1.0; //加倍?

	bool m_bCloud, 
		 m_bTown, 
		 m_bSwim, 
		 m_bFly;

	int m_nReturnMap,
		m_nForcedReturn,
		m_nMobRate,
		m_nFieldType,
		m_nFieldLimit,
		m_nCreateMobInterval,
		m_nFixedMobCapacity;

	std::string m_strFirstUserEnter, 
				m_strUserEnter;

	static void UpdateTrigger(Field* pField)
	{
		pField->Update();
	}

public:
	Field();
	~Field();

	void BroadcastPacket(OutPacket* oPacket);

	void SetCould(bool cloud);
	bool IsCloud() const;

	void SetTown(bool town);
	bool IsTown() const;

	void SetSwim(bool swim);
	bool IsSwim() const;

	void SetFly(bool fly);
	bool IsFly() const;

	void SetFieldID(int nFieldID);
	int GetFieldID() const;

	void SetReturnMap(int returnMap);
	int GetReturnMap() const;

	void SetForcedReturn(int forcedReturn);
	int GetForcedReturn() const;

	void SetMobRate(int mobRate);
	int GetMobRate() const;

	void SetFieldType(int fieldType);
	int GetFieldType() const;

	void SetFieldLimit(int fieldLimit);
	int GetFieldLimit() const;

	void SetCreateMobInterval(int interval);
	int GetCreateMobInterval() const;

	void SetFiexdMobCapacity(int capacity);
	int GetFixedMobCapacity() const;

	void SetFirstUserEnter(const std::string& script);
	const std::string& GetFirstUserEnter() const;

	void SetUserEnter(const std::string& script);
	const std::string& GetUserEnter() const;

	void SetMapSize(int x, int y);
	const FieldPoint& GetMapSize() const;
	void SetLeftTop(int x, int y);
	const FieldPoint& GetLeftTop() const;

	void SetFieldSet(FieldSet *pFieldSet);
	FieldSet *GetFieldSet();

	void InitLifePool();

	LifePool *GetLifePool();
	DropPool *GetDropPool();

	void OnEnter(User *pUser);
	void OnLeave(User *pUser);

	//發送oPacket給該地圖的其他User，其中pExcept是例外對象
	void SplitSendPacket(OutPacket* oPacket, User* pExcept);

	void OnMobMove(User* pCtrl, Mob* pMob, InPacket* iPacket);
	//不同Field 自行處理
	virtual void OnPacket(User* pUser, InPacket* iPacket);
	void OnUserMove(User* pUser, InPacket *iPacket);

	PortalMap* GetPortalMap();
	TownPortalPool* GetTownPortalPool();
	ReactorPool* GetReactorPool();
	SummonedPool* GetSummonedPool();

	std::mutex& GetFieldLock();

	WvsPhysicalSpace2D* GetSpace2D();

	void Update();
};

