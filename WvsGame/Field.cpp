#include "Field.h"
#include "LifePool.h"
#include <mutex>

std::mutex fieldUserMutex;

Field::Field()
	: pLifePool(new LifePool)
{
}

Field::~Field()
{
}

void Field::OnEnter(User *pUser)
{
	std::lock_guard<std::mutex> userGuard(fieldUserMutex);
	mUser[pUser->GetUserID()] = pUser;
	pLifePool->OnEnter(pUser);
}

void Field::InitLifePool()
{
	pLifePool->Init(nFieldID);
}

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

}