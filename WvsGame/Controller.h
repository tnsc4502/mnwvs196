#pragma once
#include <vector>
#include <set>
#include <algorithm>

class User;
class Mob;
class Npc;

class Controller
{
	std::vector<Mob*> m_lCtrlMob;
	std::vector<Npc*> m_lCtrlNpc;

	User *pUser;
public:
	Controller(User *ptrUser);
	~Controller();

	User* GetUser();

	int GetTotalControlledCount() const;

	int GetMobCtrlCount() const;
	void AddCtrlMob(Mob* ctrl);
	void RemoveCtrlMob(Mob* ctrl);

	int GetNpcCtrlCount() const;
	void AddCtrlNpc(Npc* ctrl);
	void RemoveCtrlNpc(Npc* ctrl);

	std::vector<Mob*>& GetMobCtrlList();
};

