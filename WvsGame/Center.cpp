#include "Center.h"
#include <functional>
#include <thread>

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\GamePacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"

#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Random\Rand32.h"

#include "..\WvsLib\Common\ServerConstants.hpp"

#include "WvsGame.h"
#include "User.h"
#include "FieldMan.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

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

void Center::OnNotifyCenterDisconnected(SocketBase *pSocket)
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsLogin][Center]與Center Server中斷連線。\n");
}

void Center::OnConnected()
{
	WvsLogger::LogRaw("[WvsGame][Center::OnConnect]Center Server 認證完成，與世界伺服器連線成功建立。\n");

	//向Center Server發送Hand Shake封包
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RegisterCenterRequest);

	//WvsGame的ServerType為SRV_GAME
	oPacket.Encode1(ServerConstants::ServerType::SRV_GAME);

	//[+07-07] Game的頻道ID
	oPacket.Encode1((int)WvsBase::GetInstance<WvsGame>()->GetChannelID());

	//Encode IP
	auto ip = WvsBase::GetInstance<WvsGame>()->GetExternalIP();
	for (int i = 0; i < 4; ++i)
		oPacket.Encode1((unsigned char)ip[i]);

	//Encode Port
	oPacket.Encode2(WvsBase::GetInstance<WvsGame>()->GetExternalPort());

	SendPacket(&oPacket);
	OnWaitingPacket();
}

void Center::OnPacket(InPacket *iPacket)
{
	WvsLogger::LogRaw("[Center::OnPacket]");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case CenterSendPacketFlag::RegisterCenterAck:
	{
		auto result = iPacket->Decode1();
		if (!result)
		{
			WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[Warning]The Center Server Didn't Accept This Socket. Program Will Terminated.\n");
			exit(0);
		}
		WvsLogger::LogRaw("Center Server Authenciated Ok. The Connection Between Local Server Has Builded.\n");
		break;
	}
	case CenterSendPacketFlag::CenterMigrateInResult:
		OnCenterMigrateInResult(iPacket);
		break;
	case CenterSendPacketFlag::MigrateCashShopResult:
	case CenterSendPacketFlag::TransferChannelResult:
		OnTransferChannelResult(iPacket);
		break;
	}
}

void Center::OnClosed()
{
}

void Center::OnConnectFailed()
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsGame][Center::OnConnect]Center Server拒絕當前LocalServer連接，程式即將終止。\n");
	OnDisconnect();
}

void Center::OnCenterMigrateInResult(InPacket *iPacket)
{
	unsigned int nClientSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsGame>()->GetSocket(nClientSocketID);
	OutPacket oPacket;
	oPacket.Encode2(GameSendPacketFlag::Client_SetFieldStage);
	oPacket.Encode4(WvsBase::GetInstance<WvsGame>()->GetChannelID()); //Channel ID
	oPacket.Encode1(0);
	oPacket.Encode4(0);
	oPacket.Encode1(1); //bCharacterData
	oPacket.Encode4(0);
	oPacket.Encode4(800);
	oPacket.Encode4(600);
	oPacket.Encode1(1); //bCharacterData
	oPacket.Encode2(0);

	oPacket.Encode4((unsigned int)Rand32::GetInstance()->Random());
	oPacket.Encode4((unsigned int)Rand32::GetInstance()->Random());
	oPacket.Encode4((unsigned int)Rand32::GetInstance()->Random());

	auto deleter = [](User* p) { FreeObj(p); };
	std::shared_ptr <User> pUser{ 
		AllocObjCtor(User)((ClientSocket*)pSocket, iPacket),
		deleter
	};
	pUser->EncodeCharacterData(&oPacket);

	for (int i = 0; i < 5; ++i)
		oPacket.Encode4(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode8(GameDateTime::GetCurrentDate()); //TIME

	oPacket.EncodeHexString("64 00 00 00 00 00 00 01 A6 00 00 00 03 00 00 00 83 7D 26 5A 02 00 00 24 66 00 00 00 00 00 03 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 40 E0 FD 3B 37 4F 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 82 16 FB 52 01 00 00 24 0C 00 00 00 00 00 00 00 00 00 00 00 C8 00 00 00 F7 24 11 76 00 00 00 24 0C 00 00 00 01 00 00 24 02 00 00 24 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 96 00 00 00 00");
	//oPacket.Print();
	pSocket->SendPacket(&oPacket);
	WvsBase::GetInstance<WvsGame>()->OnUserConnected(pUser);
	pUser->OnMigrateIn();
}

void Center::OnTransferChannelResult(InPacket * iPacket)
{
	unsigned int nClientSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsGame>()->GetSocket(nClientSocketID);
	OutPacket oPacket;
	bool bSuccess = iPacket->Decode1() == 1 ? true : false;
	if (bSuccess)
	{
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnTransferChannel);
		
		// 7 = Header(2) + nClientSocketID(4) + bSuccess(1)
		oPacket.EncodeBuffer(iPacket->GetPacket() + 7, iPacket->GetPacketSize() - 7);
	}
	else
	{
		oPacket.Encode2(FieldSendPacketFlag::Field_OnTransferChannelReqIgnored);
		oPacket.Encode1(1);
	}
	pSocket->SendPacket(&oPacket);
}

