#include "WvsWorld.h"
#include "UserTransferStatus.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

WvsWorld::WvsWorld()
{
}


WvsWorld::~WvsWorld()
{
}

void WvsWorld::InitializeWorld()
{
	m_WorldInfo.nEventType = ConfigLoader::GetInstance()->IntValue("EventType");
	m_WorldInfo.nWorldID = ConfigLoader::GetInstance()->IntValue("WorldID");
	m_WorldInfo.strEventDesc = ConfigLoader::GetInstance()->StrValue("EventDesc");
	m_WorldInfo.strWorldDesc = ConfigLoader::GetInstance()->StrValue("WorldDesc");
}

void WvsWorld::SetUserTransferStatus(int nUserID, UserTransferStatus* pStatus)
{
	auto deleter = [](UserTransferStatus *p) { FreeObj(p); };
	m_mUserTransferStatus[nUserID].reset(pStatus, deleter);
}

const UserTransferStatus* WvsWorld::GetUserTransferStatus(int nUserID) const
{
	auto findIter = m_mUserTransferStatus.find(nUserID);
	if (findIter == m_mUserTransferStatus.end())
		return nullptr;
	return (findIter->second.get());
}

void WvsWorld::ClearUserTransferStatus(int nUserID)
{
	m_mUserTransferStatus.erase(nUserID);
}

const WorldInfo & WvsWorld::GetWorldInfo() const
{
	return m_WorldInfo;
}
