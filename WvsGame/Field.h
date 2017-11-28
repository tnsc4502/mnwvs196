#pragma once
#include <map>
#include <functional>
#include "User.h"
#include "Task\AsnycScheduler.h"

class LifePool;
class Mob;
class Portal;
class PortalMap;
class TownPortalPool;

class Field
{
	std::map<int, User*> m_mUser; //m_lUser in WvsGame.idb
	int m_nFieldID = 0;
	LifePool *m_pLifePool;
	PortalMap *m_pPortalMap;
	TownPortalPool *m_pTownPortalPool;

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
		m_nFixedMobCapacity, 
		m_nMapSizeX, 
		m_nMapSizeY;

	std::string m_strFirstUserEnter, 
				m_strUserEnter;

	static void UpdateTrigger(Field* pField)
	{
		pField->Update();
	}

	std::_Binder<std::_Unforced, void(*)(Field* pField), Field* const> m_updateBinder;
	AsnycScheduler::AsnycScheduler<decltype(m_updateBinder)>* m_asyncUpdateTimer;

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

	void SetMapSizeX(int x);
	int GetMapSizeX();

	void SetMapSizeY(int y);
	int GetMapSizeY();

	void InitLifePool();

	LifePool *GetLifePool();

	void OnEnter(User *pUser);
	void OnLeave(User *pUser);

	//發送oPacket給該地圖的其他User，其中pExcept是例外對象
	void SplitSendPacket(OutPacket* oPacket, User* pExcept);

	void OnMobMove(User* pCtrl, Mob* pMob, InPacket* iPacket);
	//不同Field 自行處理
	virtual void OnPacket(User* pUser, InPacket* iPacket);

	PortalMap* GetPortalMap();
	TownPortalPool* GetTownPortalPool();

	void Update();
};

