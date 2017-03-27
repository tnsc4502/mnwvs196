#include "LocalServer.h"
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

#include "Net\PacketFlags\LoginPacketFlags.hpp"
#include "Net\PacketFlags\CenterPacketFlags.hpp"
#include "Net\PacketFlags\GamePacketFlags.hpp"

#include "Constants\ServerConstants.hpp"
#include "WvsCenter.h"

#include "..\Database\CharacterDBAccessor.h"

LocalServer::LocalServer(asio::io_service& serverService)
	: SocketBase(serverService, true)
{
}

LocalServer::~LocalServer()
{
}

void LocalServer::OnClosed()
{

}

void LocalServer::OnPacket(InPacket *iPacket)
{
	printf("[ServerBase::OnPacket]");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case LoginPacketFlag::RegisterCenterRequest:
		OnRegisterCenterRequest(iPacket);
		break;
	case LoginPacketFlag::RequestCharacterList:
		OnRequestCharacterList(iPacket);
		break;
	case LoginPacketFlag::RequestCreateNewCharacter:
		OnRequestCreateNewCharacter(iPacket);
		break;
	case LoginPacketFlag::RequestGameServerInfo:
		OnRequestGameServerInfo(iPacket);
		break;
	case GamePacketFlag::RequestMigrateIn:
		OnRequestMigrateIn(iPacket);
		break;
	}
}

void LocalServer::OnRegisterCenterRequest(InPacket *iPacket)
{
	auto serverType = iPacket->Decode1();
	SetServerType(serverType);
	printf("Accept New %s\n", (serverType == ServerConstants::SVR_LOGIN ? "WvsLogin" : "WvsGame"));

	if (serverType == ServerConstants::SVR_GAME)
	{
		WvsBase::GetInstance<WvsCenter>()->RegisterChannel(shared_from_this(), iPacket);
		WvsBase::GetInstance<WvsCenter>()->NotifyWorldChanged();
	}

	OutPacket oPacket;
	oPacket.Encode2(CenterPacketFlag::RegisterCenterAck);
	oPacket.Encode1(1); //Success;
	if (serverType == ServerConstants::SVR_LOGIN)
	{
		auto pWorld = WvsBase::GetInstance<WvsCenter>();
		oPacket.Encode1(pWorld->GetWorldInfo().nWorldID);
		oPacket.Encode1(pWorld->GetWorldInfo().nEventType);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strWorldDesc);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strEventDesc);
		printf("[LocalServer::OnRegisterCenterRequest]Encoding World Information.\n");
	}
	SendPacket(&oPacket);
}

void LocalServer::OnRequestCharacterList(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostLoadCharacterListRequest(this, nLoginSocketID, nAccountID, WvsBase::GetInstance<WvsCenter>()->GetWorldInfo().nWorldID);
}

void LocalServer::OnRequestCreateNewCharacter(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();

	std::string strName = iPacket->DecodeStr();
	iPacket->Decode4();
	iPacket->Decode4();

	int nJobType = iPacket->Decode4();
	int nSubJob = iPacket->Decode2();
	unsigned char nGender = iPacket->Decode1();
	unsigned char nSkin = iPacket->Decode1();
	iPacket->Decode1();
	int nFace = iPacket->Decode4();
	int nHair = iPacket->Decode4();

	int nTopID = iPacket->Decode4();
	int nShoesID = iPacket->Decode4();
	int nWeaponID = iPacket->Decode4();

	int aEquips[CharacterDBAccessor::EQP_ID_FLAG_END] = { 0 };
	int aStats[CharacterDBAccessor::STAT_FLAG_END] = { 0 };

	CharacterDBAccessor::GetDefaultCharacterStat(aStats);

	aEquips[CharacterDBAccessor::EQP_ID_TopEquip] = nTopID;
	aEquips[CharacterDBAccessor::EQP_ID_ShoesEquip] = nShoesID;
	aEquips[CharacterDBAccessor::EQP_ID_WeaponEquip] = nWeaponID;

	CharacterDBAccessor::GetInstance()->PostCreateNewCharacterRequest(
		this, 
		nLoginSocketID, 
		nAccountID, 
		WvsBase::GetInstance<WvsCenter>()->GetWorldInfo().nWorldID, 
		strName, 
		nGender, 
		nFace, 
		nHair, 
		nSkin, 
		(const int*)aEquips, 
		(const int*)aStats);
}

void LocalServer::OnRequestGameServerInfo(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nWorldID = iPacket->Decode4();
	if (nWorldID != WvsBase::GetInstance<WvsCenter>()->GetWorldInfo().nWorldID)
	{
		printf("[LocalServer::OnRequstGameServerInfo]Error! Client trying to connect to an unknown world.\n");
		return;
	}
	int nChannelID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	
	OutPacket oPacket;
	oPacket.Encode2(CenterPacketFlag::GameServerInfoResponse);
	oPacket.Encode4(nLoginSocketID);
	oPacket.Encode2(0);
	oPacket.Encode4(WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID).GetExternalIP());
	oPacket.Encode2(WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID).GetExternalPort());
	oPacket.Encode4(0);
	oPacket.Encode2(0);
	oPacket.Encode4(0);
	oPacket.Encode2(0);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(0);
	oPacket.Encode4(0);

	SendPacket(&oPacket);
}

void LocalServer::OnRequestMigrateIn(InPacket *iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostCharacterDataRequest(this, nClientSocketID, nCharacterID);
}