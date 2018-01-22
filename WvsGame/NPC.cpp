#include "Npc.h"
#include "User.h"
#include "..\Common\Net\InPacket.h"
#include "..\Common\Net\OutPacket.h"

Npc::Npc()
{
}

Npc::~Npc()
{
}

void Npc::OnUpdateLimitedInfo(User * pUser, InPacket * iPacket)
{
	int nRemained = iPacket->RemainedCount();
	OutPacket oPacket;
	oPacket.Encode2(0x417);
	printf("[LifePool::OnNpcPacket][OnUpdateLimitedInfo]Remained = %d\n", nRemained);
	if (nRemained == 6)
	{
		oPacket.Encode4(GetFieldObjectID());
		oPacket.Encode2(iPacket->Decode2());
		oPacket.Encode4(iPacket->Decode4());
	}
	else
		return;
	pUser->SendPacket(&oPacket);
}

void Npc::SendChangeControllerPacket(User * pUser)
{
	OutPacket oPacket;
	oPacket.Encode2(0x415);
	oPacket.Encode1(1);
	oPacket.Encode4(GetFieldObjectID());
	oPacket.Encode4(GetTemplateID());
	EncodeInitData(&oPacket);
	pUser->SendPacket(&oPacket);
}

void Npc::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2(0x412); //CNpcPool::OnUserEnterField
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode4(GetTemplateID());
	EncodeInitData(oPacket);
}

void Npc::MakeLeaveFieldPacket(OutPacket * oPacket)
{
}

void Npc::EncodeInitData(OutPacket *oPacket)
{
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetCy());
	oPacket->Encode1(0);
	oPacket->Encode1(~GetF());
	oPacket->Encode2(GetFh());
	oPacket->Encode2(GetRx0());
	oPacket->Encode2(GetRx1());
	oPacket->Encode1(!IsHidden());
	oPacket->Encode4(0);
	oPacket->Encode1(0);
	oPacket->Encode4(-1);
	oPacket->Encode4(0);

	oPacket->Encode4(0);
	oPacket->Encode4(0);

	oPacket->EncodeStr("");
	oPacket->Encode1(0);
}