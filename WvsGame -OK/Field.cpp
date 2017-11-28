#include "Field.h"
#include "LifePool.h"
#include "MobPacketFlags.h"
#include "Net\InPacket.h"

#include <mutex>
#include <functional>

std::mutex fieldUserMutex;

Field::Field()
	: pLifePool(new LifePool), m_updateBinder(std::bind(&Field::UpdateTrigger, this))
{
	m_asyncUpdateTimer = AsnycScheduler::CreateTask(m_updateBinder, 1000, true);
	//this->m_asyncUpdateTimer = (void*)timer;
	//InitLifePool();
}

Field::~Field()
{
}

void Field::BroadcastPacket(OutPacket * oPacket)
{
	std::lock_guard<std::mutex> userGuard(fieldUserMutex);
	for (auto& user : mUser)
		user.second->SendPacket(oPacket);
}

void Field::SetCould(bool cloud)
{
	m_bCloud = cloud;
}

bool Field::IsCloud() const
{
	return m_bCloud;
}

void Field::SetTown(bool town)
{
	m_bTown = town;
}

bool Field::IsTown() const
{
	return m_bTown;
}

void Field::SetSwim(bool swim)
{
	m_bSwim = swim;
}

bool Field::IsSwim() const
{
	return m_bSwim;
}

void Field::SetFly(bool fly)
{
	m_bFly = fly;
}

bool Field::IsFly() const
{
	return m_bFly;
}

void Field::SetFieldID(int nFieldID)
{
	this->nFieldID = nFieldID;
}

int Field::GetFieldID() const
{
	return nFieldID;
}

void Field::SetReturnMap(int returnMap)
{
	m_nReturnMap = returnMap;
}

int Field::GetReturnMap() const
{
	return m_nReturnMap;
}

void Field::SetForcedReturn(int forcedReturn)
{
	m_nForcedReturn = forcedReturn;
}

int Field::GetForcedReturn() const
{
	return m_nForcedReturn;
}

void Field::SetMobRate(int mobRate)
{
	m_nMobRate = mobRate;
}

int Field::GetMobRate() const
{
	return m_nMobRate;
}

void Field::SetFieldType(int fieldType)
{
	m_nFieldType = fieldType;
}

int Field::GetFieldType() const
{
	return m_nFieldType;
}

void Field::SetFieldLimit(int fieldLimit)
{
	m_nFieldLimit = fieldLimit;
}

int Field::GetFieldLimit() const
{
	return m_nFieldLimit;
}

void Field::SetCreateMobInterval(int interval)
{
	m_nCreateMobInterval = interval;
}

int Field::GetCreateMobInterval() const
{
	return m_nCreateMobInterval;
}

void Field::SetFiexdMobCapacity(int capacity)
{
	m_nFixedMobCapacity = capacity;
}

int Field::GetFixedMobCapacity() const
{
	return m_nFixedMobCapacity;
}

void Field::SetFirstUserEnter(const std::string & script)
{
	m_strFirstUserEnter = script;
}

const std::string & Field::GetFirstUserEnter() const
{
	return m_strFirstUserEnter;
}

void Field::SetUserEnter(const std::string & script)
{
	m_strUserEnter = script;
}

void Field::SetMapSizeX(int x)
{
	m_nMapSizeX = x;
}

int Field::GetMapSizeX()
{
	return m_nMapSizeX;
}

void Field::SetMapSizeY(int y)
{
	m_nMapSizeY = y;
}

int Field::GetMapSizeY()
{
	return m_nMapSizeY;
}

const std::string & Field::GetUserEnter() const
{
	return m_strUserEnter;
}

void Field::InitLifePool()
{
	pLifePool->Init(this, nFieldID);
}

LifePool * Field::GetLifePool()
{
	return pLifePool;
}

void Field::OnEnter(User *pUser)
{
	std::lock_guard<std::mutex> userGuard(fieldUserMutex);
	if (!m_asyncUpdateTimer->IsStarted())
		m_asyncUpdateTimer->Start();
	mUser[pUser->GetUserID()] = pUser;
	pLifePool->OnEnter(pUser);
}

void Field::OnLeave(User * pUser)
{
	std::lock_guard<std::mutex> userGuard(fieldUserMutex);
	mUser.erase(pUser->GetUserID());
	if (mUser.size() == 0 && m_asyncUpdateTimer->IsStarted())
		m_asyncUpdateTimer->Abort();
}

//發送oPacket給該地圖的其他User，其中pExcept是例外對象
void Field::SplitSendPacket(OutPacket *oPacket, User *pExcept)
{
	std::lock_guard<std::mutex> userGuard(fieldUserMutex);
	for (auto& user : mUser)
	{
		if ((pExcept == nullptr) || user.second->GetUserID() != pExcept->GetUserID())
			user.second->SendPacket(oPacket);
	}
}

void Field::OnPacket(InPacket *iPacket)
{
	short nHeader = iPacket->Decode2();
	printf("Field Packet Received %d.\n", (int)nHeader);
	/*if (nHeader >= MobRecvPacketFlag::MobRecvPacketFlag::minFlag && nHeader <= MobRecvPacketFlag::MobRecvPacketFlag::maxFlag)
	{
		printf("Mob Packet Received %d.\n", (int)nHeader);
	}*/

}

void Field::Update()
{
	//printf("Field Update Called\n");
	//pLifePool->Update();
}
