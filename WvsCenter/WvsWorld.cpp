#include "WvsWorld.h"
#include "UserTransferStatus.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

WvsWorld::WvsWorld()
{
}


WvsWorld::~WvsWorld()
{
}

void WvsWorld::SetConfigLoader(ConfigLoader * pCfg)
{
	m_pCfgLoader = pCfg;
}

void WvsWorld::InitializeWorld()
{
	m_WorldInfo.nEventType = m_pCfgLoader->IntValue("EventType");
	m_WorldInfo.nWorldID = m_pCfgLoader->IntValue("WorldID");
	m_WorldInfo.strEventDesc = m_pCfgLoader->StrValue("EventDesc");
	m_WorldInfo.strWorldDesc = m_pCfgLoader->StrValue("WorldDesc");
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
