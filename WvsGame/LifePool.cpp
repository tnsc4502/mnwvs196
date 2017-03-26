#include "LifePool.h"
#include "..\Common\Net\OutPacket.h"
#include "User.h"

LifePool::LifePool()
{
}


LifePool::~LifePool()
{
}

void LifePool::Init(int nFieldID)
{
	auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldID / 100000000)][std::to_string(nFieldID)];
	auto& lifeData = mapWz["life"];
	for (auto& node : lifeData)
	{
		const auto &typeFlag = (std::string)node["type"];
		if (typeFlag == "n")
			CreateNpc(node);
		else if (typeFlag == "m")
			CreateMob(node);
	}
	mapWz.ReleaseData();
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
	pFieldObj->SetFieldObjectID(++atomicObjectCounter);
	pFieldObj->SetTemplateID(atoi(((std::string)dataNode["id"]).c_str()));
}

void LifePool::CreateNpc(WZ::Node& dataNode)
{
	Npc npc;
	SetFieldObjAttribute(&npc, dataNode);
	mNpcMap[npc.GetTemplateID()] = npc;
}

void LifePool::CreateMob(WZ::Node& dataNode)
{

}

void LifePool::OnEnter(User *pUser)
{
	for (auto& npc : mNpcMap)
	{
		OutPacket oPacket;
		npc.second.MakeEnterFieldPacket(&oPacket);
		pUser->SendPacket(&oPacket);
	}
}