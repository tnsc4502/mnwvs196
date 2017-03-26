#pragma once
#include <map>
#include "Npc.h"
#include "Mob.h"
#include "..\Common\Wz\WzResMan.hpp"
#include <atomic>

class User;

class LifePool
{
	std::atomic<int> atomicObjectCounter = 0x1000;

	std::map<int, Npc> mNpcMap;
	std::map<int, Mob> mMobMap;

	void SetFieldObjAttribute(FieldObj* pFieldObj, WZ::Node& dataNode);

public:
	LifePool();
	~LifePool();

	void CreateNpc(WZ::Node& dataNode);
	void CreateMob(WZ::Node& dataNode);
	
	void PrintNpc()
	{
		for (auto& x : mNpcMap)
			std::cout << x.second.GetTemplateID() << std::endl;
	}

	void Init(int nFieldID);
	void OnEnter(User *pUser);
};

