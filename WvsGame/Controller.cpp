#include "Controller.h"

Controller::Controller(User *ptrUser)
	: pUser(ptrUser)
{
}

Controller::~Controller()
{
}

User* Controller::GetUser()
{
	return pUser;
}

int Controller::GetMobCtrlCount() const
{
	return (int)m_lCtrlMob.size();
}

int Controller::GetTotalControlledCount() const
{
	return GetMobCtrlCount() + GetNpcCtrlCount();
}

void Controller::AddCtrlMob(Mob * ctrl)
{
	m_lCtrlMob.insert(ctrl);
}

void Controller::RemoveCtrlMob(Mob * ctrl)
{
	auto result = std::find(m_lCtrlMob.begin(), m_lCtrlMob.end(), ctrl);
	if (result != m_lCtrlMob.end())
		m_lCtrlMob.erase(result);
}

int Controller::GetNpcCtrlCount() const
{
	return (int)m_lCtrlNpc.size();
}

void Controller::AddCtrlNpc(Npc* ctrl)
{
	m_lCtrlNpc.insert(ctrl);
}

void Controller::RemoveCtrlNpc(Npc * ctrl)
{
	auto result = std::find(m_lCtrlNpc.begin(), m_lCtrlNpc.end(), ctrl);
	if (result != m_lCtrlNpc.end())
		m_lCtrlNpc.erase(result);
}

std::set<Mob*>& Controller::GetMobCtrlList()
{
	return m_lCtrlMob;
}
