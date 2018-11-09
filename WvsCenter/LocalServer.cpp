
#include "..\Database\CharacterDBAccessor.h"
#include "LocalServer.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\ShopPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\GamePacketFlags.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"
#include "WvsCenter.h"
#include "WvsWorld.h"
#include "UserTransferStatus.h"
#include "..\WvsGame\ItemInfo.h"

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
	WvsLogger::LogRaw("[WvsCenter][LocalServer::OnPacket]封包接收：");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case LoginSendPacketFlag::Center_RegisterCenterRequest:
			OnRegisterCenterRequest(iPacket);
			break;
		case LoginSendPacketFlag::Center_RequestCharacterList:
			OnRequestCharacterList(iPacket);
			break;
		case LoginSendPacketFlag::Center_RequestCreateNewCharacter:
			OnRequestCreateNewCharacter(iPacket);
			break;
		case LoginSendPacketFlag::Center_RequestGameServerInfo:
			OnRequestGameServerInfo(iPacket);
			break;
		case GameSendPacketFlag::RequestMigrateIn:
			OnRequestMigrateIn(iPacket);
			break;
		case ShopInternalPacketFlag::RequestMigrateOut:
		case GameSendPacketFlag::RequestMigrateOut:
			OnRequestMigrateOut(iPacket);
			break;
		case ShopInternalPacketFlag::RequestTransferToGame:
		case GameSendPacketFlag::RequestTransferChannel:
			OnRequestTransferChannel(iPacket);
			break;
		case GameSendPacketFlag::RequestTransferShop:
			OnRequestMigrateCashShop(iPacket);
			break;
		case ShopInternalPacketFlag::RequestBuyCashItem:
			OnRequestBuyCashItem(iPacket);
			break;
		case ShopInternalPacketFlag::RequestLoadLocker:
			OnRequestLoadLocker(iPacket);
			break;
		case ShopInternalPacketFlag::RequestUpdateCash:
			OnReuqestUpdateCash(iPacket);
			break;
		case ShopInternalPacketFlag::RequestMoveLToS:
			OnReuqestMoveLockerToSlot(iPacket);
			break;
		case ShopInternalPacketFlag::RequestMoveSToL:
			OnReuqestMoveSlotToLocker(iPacket);
			break;
	}
}

void LocalServer::OnRegisterCenterRequest(InPacket *iPacket)
{
	auto serverType = iPacket->Decode1();
	SetServerType(serverType);
	const char* pInstanceName = (serverType == ServerConstants::SRV_LOGIN ? "WvsLogin" : (serverType == ServerConstants::SRV_GAME ? "WvsGame" : "WvsShop"));
	WvsLogger::LogFormat("[WvsCenter][LocalServer::OnRegisterCenterRequest]收到新的[%s][%d]連線請求。\n", pInstanceName, serverType);

	if (serverType == ServerConstants::SRV_GAME)
	{
		WvsBase::GetInstance<WvsCenter>()->RegisterChannel(shared_from_this(), iPacket);
		WvsBase::GetInstance<WvsCenter>()->NotifyWorldChanged();
	}

	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::RegisterCenterAck);
	oPacket.Encode1(1); //Success;
	if (serverType == ServerConstants::SRV_LOGIN)
	{
		auto pWorld = WvsWorld::GetInstance();
		oPacket.Encode1(pWorld->GetWorldInfo().nWorldID);
		oPacket.Encode1(pWorld->GetWorldInfo().nEventType);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strWorldDesc);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strEventDesc);
		WvsBase::GetInstance<WvsCenter>()->NotifyWorldChanged();
		//printf("[LocalServer::OnRegisterCenterRequest]Encoding World Information.\n");
	}
	
	if (serverType == ServerConstants::SRV_SHOP)
		WvsBase::GetInstance<WvsCenter>()->RegisterCashShop(shared_from_this(), iPacket);

	SendPacket(&oPacket);
}

void LocalServer::OnRequestCharacterList(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();
	int nChannelID = iPacket->Decode1();
	if(WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID) != nullptr)
		CharacterDBAccessor::GetInstance()->PostLoadCharacterListRequest(this, nLoginSocketID, nAccountID, WvsWorld::GetInstance()->GetWorldInfo().nWorldID);
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
	int nAttr = iPacket->Decode1(), 
		nAttrRead = 0,
		nFaceID = 0,
		nHairID = 0;

	int aEquips[CharacterDBAccessor::EQP_ID_FLAG_END] = { 0 };
	int aStats[CharacterDBAccessor::STAT_FLAG_END] = { 0 };

	for (int i = 0; i < nAttr; ++i)
	{
		nAttrRead = iPacket->Decode4();
		if (ItemInfo::IsWeapon(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_WeaponEquip] = nAttrRead;
		else if (ItemInfo::IsPants(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_PantsEquip] = nAttrRead;
		else if (ItemInfo::IsCoat(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_CoatEquip] = nAttrRead;
		else if (ItemInfo::IsCap(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_CapEquip] = nAttrRead;
		else if (ItemInfo::IsCape(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_CapeEquip] = nAttrRead;
		else if (ItemInfo::IsFace(nAttrRead))
			nFaceID = nAttrRead;
		else if (ItemInfo::IsHair(nAttrRead))
			nHairID = nAttrRead;
		else if (ItemInfo::IsShield(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_ShieldEquip] = nAttrRead;
		else if (ItemInfo::IsShoes(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_ShoesEquip] = nAttrRead;
		else if (ItemInfo::IsLongcoat(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_CoatEquip] = nAttrRead;
	}

	CharacterDBAccessor::GetDefaultCharacterStat(aStats);
	CharacterDBAccessor::GetInstance()->PostCreateNewCharacterRequest(
		this, 
		nLoginSocketID, 
		nAccountID, 
		WvsWorld::GetInstance()->GetWorldInfo().nWorldID, 
		strName, 
		nGender, 
		nFaceID,
		nHairID,
		nSkin, 
		(const int*)aEquips, 
		(const int*)aStats);
}

void LocalServer::OnRequestGameServerInfo(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nWorldID = iPacket->Decode4();
	if (nWorldID != WvsWorld::GetInstance()->GetWorldInfo().nWorldID)
	{
		WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsCenter][LocalServer::OnRequstGameServerInfo]異常：客戶端嘗試連線至不存在的頻道伺服器[WvsGame]。\n");
		return;
	}
	int nChannelID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	
	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::GameServerInfoResponse);
	oPacket.Encode4(nLoginSocketID);
	oPacket.Encode2(0);
	oPacket.Encode4(WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID)->GetExternalIP());
	oPacket.Encode2(WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID)->GetExternalPort());
	oPacket.Encode4(0);
	oPacket.Encode2(0);
	oPacket.Encode4(0);
	oPacket.Encode2(0);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(0);
	oPacket.Encode4(0);

	WvsWorld::GetInstance()->ClearUserTransferStatus(nCharacterID);
	SendPacket(&oPacket);
}

void LocalServer::OnRequestMigrateIn(InPacket *iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::CenterMigrateInResult);
	oPacket.Encode4(nClientSocketID);
	CharacterDBAccessor::GetInstance()->PostCharacterDataRequest(this, nClientSocketID, nCharacterID, &oPacket); // for WvsGame
	auto pUserTransferStatus = WvsWorld::GetInstance()->GetUserTransferStatus(nCharacterID);

	WvsLogger::LogFormat("OnRequestMigrateIn, has transfer status ? %d\n", (pUserTransferStatus != nullptr));
	if (pUserTransferStatus == nullptr)
		oPacket.Encode1(0);
	else
	{
		oPacket.Encode1(1);
		pUserTransferStatus->Encode(&oPacket);
		//WvsWorld::GetInstance()->ClearUserTransferStatus(nCharacterID);
	}
	//CharacterDBAccessor::GetInstance()->PostCharacterDataRequest(this, nClientSocketID, nCharacterID, &oPacket); // for Client
	this->SendPacket(&oPacket);
}

void LocalServer::OnRequestMigrateOut(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->OnCharacterSaveRequest(iPacket);
	char nGameEndType = iPacket->Decode1();
	WvsLogger::LogFormat("OnRequestMigrateOut code = %d\n", (int)nGameEndType);

	if (nGameEndType == 1) //Transfer to another game server or to the shop.
	{
		UserTransferStatus* pStatus = AllocObj( UserTransferStatus );
		pStatus->Decode(iPacket);
		WvsWorld::GetInstance()->SetUserTransferStatus(nCharacterID, pStatus);
	}
	else if(nGameEndType == 0) //Migrate out from the game server.
		WvsWorld::GetInstance()->ClearUserTransferStatus(nCharacterID);

	// nGameEndType = 2 : From shop to game server
}

void LocalServer::OnRequestTransferChannel(InPacket * iPacket)
{
	WvsLogger::LogRaw("OnRequestTransferChannel\n");
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	int nChannelID = iPacket->Decode1();
	auto pEntry = WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID);
	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::TransferChannelResult);
	oPacket.Encode4(nClientSocketID);
	oPacket.Encode1((pEntry != nullptr ? 1 : 0)); //bSuccess
	if (pEntry != nullptr)
	{
		oPacket.Encode1(1);
		oPacket.Encode4(pEntry->GetExternalIP());
		oPacket.Encode2(pEntry->GetExternalPort());
		oPacket.Encode4(0);
	}
	this->SendPacket(&oPacket);
}

void LocalServer::OnRequestMigrateCashShop(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::MigrateCashShopResult);
	oPacket.Encode4(nClientSocketID);
	auto pEntry = WvsBase::GetInstance<WvsCenter>()->GetShop();
	if (WvsBase::GetInstance<WvsCenter>()->GetShop() == nullptr)
		oPacket.Encode1(0); //bSuccess
	else
	{
		oPacket.Encode1(1);
		oPacket.Encode1(1);
		oPacket.Encode4(pEntry->GetExternalIP());
		oPacket.Encode2(pEntry->GetExternalPort());
		oPacket.Encode4(0);
	}
	this->SendPacket(&oPacket);
}

void LocalServer::OnRequestBuyCashItem(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostBuyCashItemRequest(this, nClientSocketID, nCharacterID, iPacket);
}

void LocalServer::OnRequestLoadLocker(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostLoadLockerRequest(this, nClientSocketID, nCharacterID, iPacket);
}

void LocalServer::OnReuqestUpdateCash(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostUpdateCashRequest(this, nClientSocketID, nCharacterID, iPacket);
}

void LocalServer::OnReuqestMoveLockerToSlot(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostMoveLockerToSlotRequest(this, nClientSocketID, nCharacterID, iPacket);
}

void LocalServer::OnReuqestMoveSlotToLocker(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostMoveSlotToLockerRequest(this, nClientSocketID, nCharacterID, iPacket);
}
