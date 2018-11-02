#include "Center.h"
#include <functional>
#include <thread>

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Common\ServerConstants.hpp"

#include "WvsLogin.h"
#include "..\WvsLib\Logger\WvsLogger.h"

Center::Center(asio::io_service& serverService)
	: SocketBase(serverService, true)
{
}

Center::~Center()
{
}

void Center::SetCenterIndex(int idx)
{
	nCenterIndex = idx;
}

void Center::OnConnected()
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "[WvsLogin][Center::OnConnect]成功連線到Center Server！\n");

	//向Center Server發送Hand Shake封包
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RegisterCenterRequest);

	//WvsLogin的ServerType為SVR_LOGIN
	oPacket.Encode1(ServerConstants::ServerType::SRV_LOGIN);

	SendPacket(&oPacket); 
	OnWaitingPacket();
}

void Center::OnPacket(InPacket *iPacket)
{
	WvsLogger::LogRaw("[WvsLogin][Center::OnPacket]封包接收：");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case CenterSendPacketFlag::RegisterCenterAck:
	{
		auto result = iPacket->Decode1();
		if (!result)
		{
			WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsLogin][RegisterCenterAck][錯誤]Center Server拒絕當前LocalServer連接，程式即將終止。\n");
			exit(0);
		}
		WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "[WvsLogin][RegisterCenterAck]Center Server 認證完成，與世界伺服器連線成功建立。\n");
		OnUpdateWorldInfo(iPacket);
		break;
	}
	case CenterSendPacketFlag::CenterStatChanged:
		OnUpdateChannelInfo(iPacket);
		break;
	case CenterSendPacketFlag::CharacterListResponse:
		OnCharacterListResponse(iPacket);
		break;
	case CenterSendPacketFlag::GameServerInfoResponse:
		OnGameServerInfoResponse(iPacket);

		break;
	}
}

void Center::OnClosed()
{
}

void Center::OnUpdateChannelInfo(InPacket * iPacket)
{
	m_WorldInfo.nGameCount = iPacket->Decode2();
	memset(m_WorldInfo.m_aChannelStatus, 0, sizeof(int) * ServerConstants::kMaxChannelCount);
	for (int i = 0; i < m_WorldInfo.nGameCount; ++i)
		m_WorldInfo.m_aChannelStatus[(iPacket->Decode1())] = 1;
}

void Center::OnUpdateWorldInfo(InPacket *iPacket)
{
	m_WorldInfo.nWorldID = iPacket->Decode1();
	m_WorldInfo.nEventType = iPacket->Decode1();
	m_WorldInfo.strWorldDesc = iPacket->DecodeStr();
	m_WorldInfo.strEventDesc = iPacket->DecodeStr();
	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "[WvsLogin][Center::OnUpdateWorld]遊戲伺服器世界資訊更新。\n");
}

void Center::OnConnectFailed()
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsShop][Center::OnConnect]無法連線到Center Server，可能是服務尚未啟動或者確認連線資訊。\n");
	OnDisconnect();
}

void Center::OnCharacterListResponse(InPacket *iPacket)
{
	unsigned int nLoginSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsLogin>()->GetSocket(nLoginSocketID);
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientSelectWorldResult);
	oPacket.Encode1(0);
	oPacket.EncodeStr("normal");
	oPacket.Encode4(0);
	oPacket.Encode1(0);
	oPacket.Encode4(0);
	oPacket.Encode8(0);
	oPacket.Encode1(0);

	WvsLogger::LogRaw("[WvsLogin][Center::OnCharacterListResponse]玩家擁有角色清單封包 : ");
	iPacket->Print();
	WvsLogger::LogRaw("\n");

	oPacket.EncodeBuffer(iPacket->GetPacket() + 6, iPacket->GetPacketSize() - 6);

	/*int chrSize = iPacket->Decode4();
	oPacket.Encode4(chrSize); //char size
	for (int i = 0; i < chrSize; ++i)
		oPacket.Encode4(iPacket->Decode4());

	chrSize = iPacket->Decode1();
	oPacket.Encode1(chrSize); //char size
	for (int i = 0; i < chrSize; ++i)
	{
		OnEncodingCharacterStat(&oPacket, iPacket);
		OnEncodingCharacterAvatar(&oPacket, iPacket);
		oPacket.Encode1(iPacket->Decode1());
		OnEncodingRank(&oPacket, iPacket);
	}*/

	oPacket.Encode1(0x03);
	oPacket.Encode1(0);
	oPacket.Encode4(8); //char slots

	oPacket.Encode4(0);
	oPacket.Encode4(-1);
	oPacket.Encode1(0);
	oPacket.Encode8(GameDateTime::TIME_UNLIMITED);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode4(0);
	for (int i = 0; i < 25; ++i)
		oPacket.Encode2(0);
	oPacket.Encode4(0);
	oPacket.Encode8(0);
	pSocket->SendPacket(&oPacket);
}

void Center::OnGameServerInfoResponse(InPacket *iPacket)
{
	unsigned int nLoginSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsLogin>()->GetSocket(nLoginSocketID); 
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientSelectCharacterResult);
	oPacket.EncodeBuffer(iPacket->GetPacket() + 6, iPacket->GetPacketSize() - 6);
	pSocket->SendPacket(&oPacket);
}

void Center::OnNotifyCenterDisconnected(SocketBase * pSocket)
{
	WvsLogger::LogRaw("[WvsLogin][Center]與Center Server中斷連線。\n");
}
