#include "LifePool.h"
#include "..\Common\Net\OutPacket.h"
#include "User.h"
#include "MobTemplate.h"

LifePool::LifePool()
{
}


LifePool::~LifePool()
{
}

void LifePool::Init(Field* pField, int nFieldID)
{
	auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldID / 100000000)][std::to_string(nFieldID)];
	auto& lifeData = mapWz["life"];
	for (auto& node : lifeData)
	{
		const auto &typeFlag = (std::string)node["type"];
		if (typeFlag == "n")
			LoadNpcData(node);
		else if (typeFlag == "m")
			LoadMobData(node);
	}	

	//強制生成所有NPC
	for (auto& npc : m_lNpc)
		CreateNpc(npc);

	TryCreateMob(false);
	//mapWz.ReleaseData();
}

void LifePool::SetFieldObjAttribute(FieldObj* pFieldObj, WZ::Node& dataNode)
{
	pFieldObj->SetPosX(dataNode["x"]);
	pFieldObj->SetPosY(dataNode["y"]);
	pFieldObj->SetF(dataNode["f"]);
	pFieldObj->SetHide(dataNode["hide"]);
	pFieldObj->SetFh(dataNode["fh"]);
	pFieldObj->SetCy(dataNode["cy"]);
	pFieldObj->SetRx0(dataNode["rx0"]);
	pFieldObj->SetRx1(dataNode["rx1"]);
	pFieldObj->SetTemplateID(atoi(((std::string)dataNode["id"]).c_str()));
}

void LifePool::LoadNpcData(WZ::Node& dataNode)
{
	Npc npc;
	SetFieldObjAttribute(&npc, dataNode);
	m_lNpc.push_back(npc);
}

void LifePool::LoadMobData(WZ::Node& dataNode)
{
	Mob mob;
	SetFieldObjAttribute(&mob, dataNode);
	MobTemplate::GetMobTemplate(mob.GetTemplateID());
	m_lMob.push_back(mob);
}

void LifePool::CreateNpc(const Npc& npc)
{
	Npc* newNpc = new Npc();
	*newNpc = npc; //Should notice pointer data assignment
	newNpc->SetFieldObjectID(atomicObjectCounter++);
	m_aNpcGen.insert({ newNpc->GetFieldObjectID(), newNpc });
}

void LifePool::TryCreateMob(bool reset)
{
	/*
	if reset, kill all monsters and respawns
	*/
	for (int i = 0; i < 10 - (m_aMobGen.size()); ++i)
		CreateMob(m_lMob[rand() % m_lMob.size()]);
}

void LifePool::CreateMob(const Mob& mob)
{
	Mob* newMob = new Mob;
	*newMob = mob;
	newMob->SetFieldObjectID(atomicObjectCounter++);
	m_aMobGen.insert({ newMob->GetFieldObjectID(), newMob });
}

void LifePool::OnEnter(User *pUser)
{
	for (auto& npc : m_aNpcGen)
	{
		OutPacket oPacket;
		npc.second->MakeEnterFieldPacket(&oPacket);
		pUser->SendPacket(&oPacket);
	}

	for (auto& mob : m_aMobGen)
	{
		OutPacket oPacket;
		mob.second->MakeEnterFieldPacket(&oPacket);
		pUser->SendPacket(&oPacket);
	}
}