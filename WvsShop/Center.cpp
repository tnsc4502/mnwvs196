#include "Center.h"
#include <functional>
#include <thread>

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\ShopPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"

#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"

#include "User.h"
#include "WvsShop.h"
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
	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "[WvsShop][Center::OnConnect]成功連線到Center Server！\n");

	//向Center Server發送Hand Shake封包
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RegisterCenterRequest);

	//WvsLogin的ServerType為SVR_LOGIN
	oPacket.Encode1(ServerConstants::ServerType::SRV_SHOP);

	//Encode IP
	auto ip = WvsBase::GetInstance<WvsShop>()->GetExternalIP();
	for (int i = 0; i < 4; ++i)
		oPacket.Encode1((unsigned char)ip[i]);

	//Encode Port
	oPacket.Encode2(WvsBase::GetInstance<WvsShop>()->GetExternalPort());

	SendPacket(&oPacket); 
	OnWaitingPacket();
}

void Center::OnPacket(InPacket *iPacket)
{
	WvsLogger::LogRaw("[WvsShop][Center::OnPacket]封包接收：");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case CenterSendPacketFlag::RegisterCenterAck:
		{
			auto result = iPacket->Decode1();
			if (!result)
			{
				WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsShop][RegisterCenterAck][錯誤]Center Server拒絕當前LocalServer連接，程式即將終止。\n");
				exit(0);
			}
			WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "[WvsShop][RegisterCenterAck]Center Server 認證完成，與世界伺服器連線成功建立。\n");
			//OnUpdateWorldInfo(iPacket);
			break;
		}
		case CenterSendPacketFlag::CenterMigrateInResult:
			OnCenterMigrateInResult(iPacket);
			break;
		case CenterSendPacketFlag::TransferChannelResult:
			OnCenterMigrateOutResult(iPacket);
			break;
		case CenterSendPacketFlag::CashItemResult:
		{
			int nClientSocketID = iPacket->Decode4();
			int nUserID = iPacket->Decode4();
			auto pSocket = WvsBase::GetInstance<WvsShop>()->GetSocket(nClientSocketID);
			auto pUser = User::FindUser(nUserID);
			if (pSocket && pUser)
				pUser->OnCenterCashItemResult((unsigned short)iPacket->Decode2(), iPacket);
		}
		break;
	}
}

void Center::OnClosed()
{
}

void Center::OnConnectFailed()
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsShop][Center::OnConnect]無法連線到Center Server，可能是服務尚未啟動或者確認連線資訊。\n");
	OnDisconnect();
}

const WorldInfo & Center::GetWorldInfo()
{
	return m_WorldInfo;
}

void Center::OnNotifyCenterDisconnected(SocketBase * pSocket)
{
	WvsLogger::LogRaw("[WvsLogin][Center]與Center Server中斷連線。\n");
}

void Center::OnCenterMigrateInResult(InPacket *iPacket)
{
	unsigned int nClientSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsShop>()->GetSocket(nClientSocketID);
	OutPacket oPacket;
	oPacket.Encode2(ShopSendPacketFlag::Client_SetCashShop);
	oPacket.EncodeHexString("01 00 00 00 6B FE 8F 06");
	oPacket.Encode2(0);
	oPacket.EncodeHexString("00 00 1E 00 00 00 4C 6D 54 00 06 00 00 00 19 63 3D 01 02 63 3D 01 05 63 3D 01 06 63 3D 01 0A 63 3D 01 0C 63 3D 01 64 6D 54 00 00 00 00 00 54 6D 54 00 25 00 00 00 19 D8 C9 0A 1A D8 C9 0A 1B D8 C9 0A 1C D8 C9 0A 1D D8 C9 0A 1E D8 C9 0A 1F D8 C9 0A 20 D8 C9 0A 23 D8 C9 0A 30 D8 C9 0A 61 D8 C9 0A 62 D8 C9 0A 77 D8 C9 0A 78 D8 C9 0A 79 D8 C9 0A 7A D8 C9 0A 7B D8 C9 0A 7C D8 C9 0A 84 D8 C9 0A 85 D8 C9 0A 86 D8 C9 0A 87 D8 C9 0A 88 D8 C9 0A 89 D8 C9 0A 8A D8 C9 0A 8B D8 C9 0A 8C D8 C9 0A 8D D8 C9 0A A7 D8 C9 0A A8 D8 C9 0A A9 D8 C9 0A AA D8 C9 0A AB D8 C9 0A AC D8 C9 0A AD D8 C9 0A AE D8 C9 0A AF D8 C9 0A 6C 6D 54 00 05 00 00 00 7B D7 C9 0A 7C D7 C9 0A 7D D7 C9 0A 7E D7 C9 0A 7F D7 C9 0A 9C 6D 54 00 08 00 00 00 61 C4 59 08 62 C4 59 08 63 C4 59 08 64 C4 59 08 65 C4 59 08 66 C4 59 08 67 C4 59 08 68 C4 59 08 55 6D 54 00 09 00 00 00 AC 2F 31 01 AD 2F 31 01 AE 2F 31 01 AF 2F 31 01 B0 2F 31 01 B1 2F 31 01 B7 85 FF 02 B8 85 FF 02 B9 85 FF 02 6D 6D 54 00 05 00 00 00 84 D7 C9 0A 85 D7 C9 0A 86 D7 C9 0A 87 D7 C9 0A 88 D7 C9 0A 5D 6D 54 00 05 00 00 00 3D 63 3D 01 3E 63 3D 01 3F 63 3D 01 40 63 3D 01 41 63 3D 01 9D 6D 54 00 05 00 00 00 5D C4 59 08 0F 49 5B 08 5E C4 59 08 5F C4 59 08 60 C4 59 08 4E 6D 54 00 06 00 00 00 6B D7 C9 0A 6C D7 C9 0A 6D D7 C9 0A 6E D7 C9 0A 6F D7 C9 0A 70 D7 C9 0A 66 6D 54 00 00 00 00 00 56 6D 54 00 08 00 00 00 CE 2E 31 01 CF 2E 31 01 D0 2E 31 01 D1 2E 31 01 D2 2E 31 01 D3 2E 31 01 D4 2E 31 01 D5 2E 31 01 5E 6D 54 00 03 00 00 00 91 F6 41 01 92 F6 41 01 93 F6 41 01 4F 6D 54 00 06 00 00 00 91 F6 41 01 92 F6 41 01 93 F6 41 01 94 F6 41 01 95 F6 41 01 96 F6 41 01 6F 6D 54 00 00 00 00 00 5F 6D 54 00 05 00 00 00 71 2F 31 01 72 2F 31 01 73 2F 31 01 74 2F 31 01 75 2F 31 01 48 6D 54 00 05 00 00 00 D4 2F 31 01 D5 2F 31 01 D6 2F 31 01 D7 2F 31 01 D8 2F 31 01 60 6D 54 00 04 00 00 00 6D 2F 31 01 6E 2F 31 01 6F 2F 31 01 70 2F 31 01 50 6D 54 00 06 00 00 00 61 D7 C9 0A 62 D7 C9 0A 63 D7 C9 0A 64 D7 C9 0A 65 D7 C9 0A 66 D7 C9 0A 49 6D 54 00 06 00 00 00 03 63 3D 01 04 63 3D 01 07 63 3D 01 08 63 3D 01 09 63 3D 01 0B 63 3D 01 51 6D 54 00 06 00 00 00 AE D7 C9 0A AF D7 C9 0A B0 D7 C9 0A B1 D7 C9 0A B2 D7 C9 0A B3 D7 C9 0A 71 6D 54 00 00 00 00 00 59 6D 54 00 06 00 00 00 6A 63 3D 01 6B 63 3D 01 6C 63 3D 01 6D 63 3D 01 6E 63 3D 01 6F 63 3D 01 4A 6D 54 00 05 00 00 00 4F 63 3D 01 50 63 3D 01 51 63 3D 01 52 63 3D 01 53 63 3D 01 62 6D 54 00 06 00 00 00 5C D7 C9 0A 60 D7 C9 0A 5D D7 C9 0A 5E D7 C9 0A 5B D7 C9 0A 5F D7 C9 0A 6A 6D 54 00 00 00 00 00 A2 6D 54 00 0A 00 00 00 49 3C 58 08 4A 3C 58 08 4B 3C 58 08 4C 3C 58 08 4D 3C 58 08 4E 3C 58 08 4F 3C 58 08 50 3C 58 08 51 3C 58 08 52 3C 58 08 5A 6D 54 00 03 00 00 00 85 F6 41 01 86 F6 41 01 84 F6 41 01 4B 6D 54 00 07 00 00 00 F0 2F 31 01 F1 2F 31 01 F2 2F 31 01 F3 2F 31 01 F4 2F 31 01 F5 2F 31 01 F6 2F 31 01 53 6D 54 00 06 00 00 00 9F 2F 31 01 A0 2F 31 01 A1 2F 31 01 A2 2F 31 01 A3 2F 31 01 A4 2F 31 01 02 00 00 00 00 00 00 00 F7 A4 98 00 96 95 4E 00 00 A0 C1 29 E5 82 CE 01 00 80 39 0F 01 93 CE 01 00 00 00 00 05 00 00 00 00 00 00 00 14 00 00 00 1E 00 00 00 28 00 00 00 32 00 00 00 01 00 00 00 F8 A4 98 00 C8 9D 4E 00 00 A0 C1 29 E5 82 CE 01 00 80 39 0F 01 93 CE 01 00 00 00 00 05 00 00 00 00 00 00 00 14 00 00 00 1E 00 00 00 28 00 00 00 32 00 00 00 12 00 00 00 02 A9 A4 BF 07 02 B1 A4 BF 07 02 24 0E 27 07 00 1D A3 BD 0A 00 B6 92 26 0A 00 91 85 23 0A 00 92 85 23 0A 00 93 85 23 0A 02 AC 72 F8 08 02 2E 0E 27 07 02 02 0E 27 07 02 34 0E 27 07 02 A5 A4 BF 07 02 2D 2B C1 07 02 05 3B 58 08 02 80 D1 F0 08 02 20 58 F2 08 02 E3 CE 5C 08 00 00 00 00 03 00 00 00 02 00 3F 3F 29 00 3F 3F 3F 20 3F 3F 3F 20 3F 3F 20 3F 3F 3F 3F 20 3F 3F 3F 3F 20 3F 20 3F 3F 3F 20 3F 3F 3F 3F 20 2D 2D 2D 2D 2D 3E 20 3F 3F 05 00 3F 3F 3F 20 3F 13 00 3F 3F 3F 3F 3F 21 20 3F 3F 3F 20 3F 3F 3F 20 3F 3F 3F 21 03 00 3F 3F 3F 20 00 3F 3F 3F 20 3F 3F 3F 3F 20 3F 3F 3F 3F 20 3F 3F 3F 21 20 3F 3F 20 3F 3F 20 3F 3F 3F 3F 3F 3F 7E 34 00 00 00 91 49 87 78 00 00 00 80 16 00 00 00 35 00 00 00 93 49 87 78 00 00 00 80 18 00 00 00 35 00 00 00 8D 49 87 78 00 00 00 80 7E 02 00 00 38 00 00 00 8F 49 87 78 00 00 00 80 20 00 00 00 32 00 00 00 89 49 87 78 00 00 00 80 24 00 00 00 32 00 00 00 8B 49 87 78 00 00 00 80 1E 00 00 00 32 00 00 00 85 49 87 78 00 00 00 80 1C 00 00 00 36 00 00 00 87 49 87 78 00 00 00 80 28 00 00 00 78 00 00 00 81 49 87 78 00 00 00 80 2E 00 00 00 33 00 00 00 83 49 87 78 00 00 00 80 26 00 00 00 78 00 00 00 FD 49 87 78 00 00 00 80 22 00 00 00 73 00 00 00 FF 49 87 78 00 00 00 80 2C 00 00 00 73 00 00 00 F9 49 87 78 00 00 00 80 32 00 00 00 7A 00 00 00 FB 49 87 78 00 00 00 80 2A 00 00 00 38 00 00 00 F5 49 87 78 00 00 00 80 30 00 00 00 7A 00 00 00 F7 49 87 78 00 00 00 80 1A 08 00 00 68 00 00 00 F1 49 87 78 00 00 00 80 36 00 00 00 68 00 00 00 F3 49 87 78 00 00 00 80 3E 00 00 00 31 00 00 00 ED 49 87 78 00 00 00 80 6A 00 00 00 30 00 00 00 EF 49 87 78 00 00 00 80 3C 00 00 00 30 00 00 00 E9 49 87 78 00 00 00 80 AC 00 00 00 30 00 00 00 EB 49 87 78 00 00 00 80 40 00 00 00 30 00 00 00 E5 49 87 78 00 00 00 80 42 00 00 00 31 00 00 00 E7 49 87 78 00 00 00 80 44 00 00 00 31 00 00 00 E1 49 87 78 00 00 00 80 46 00 00 00 32 00 00 00 E3 49 87 78 00 00 00 80 48 00 00 00 32 00 00 00 DD 49 87 78 00 00 00 80 4A 00 00 00 33 00 00 00 DF 49 87 78 00 00 00 80 4C 00 00 00 33 00 00 00 D9 49 87 78 00 00 00 80 4E 00 00 00 34 00 00 00 DB 49 87 78 00 00 00 80 50 00 00 00 34 00 00 00 D5 49 87 78 00 00 00 80 52 00 00 00 35 00 00 00 D7 49 87 78 00 00 00 80 54 00 00 00 35 00 00 00 D1 49 87 78 00 00 00 80 56 00 00 00 36 00 00 00 D3 49 87 78 00 00 00 80 58 00 00 00 36 00 00 00 CD 49 87 78 00 00 00 80 5A 00 00 00 37 00 00 00 CF 49 87 78 00 00 00 80 5C 00 00 00 37 00 00 00 C9 49 87 78 00 00 00 80 5E 00 00 00 38 00 00 00 CB 49 87 78 00 00 00 80 60 00 00 00 38 00 00 00 C5 49 87 78 00 00 00 80 62 00 00 00 39 00 00 00 C7 49 87 78 00 00 00 80 64 00 00 00 39 00 00 00 C1 49 87 78 00 00 00 80 84 00 00 00 30 00 00 00 C3 49 87 78 00 00 00 80 68 00 00 00 30 00 00 00 3D 48 87 78 00 00 00 80 66 00 00 00 30 00 00 00 3F 48 87 78 00 00 00 80 B0 00 00 00 30 00 00 00 39 48 87 78 00 00 00 80 6E 00 00 00 30 00 00 00 3B 48 87 78 00 00 00 80 70 00 00 00 31 00 00 00 35 48 87 78 00 00 00 80 72 00 00 00 31 00 00 00 37 48 87 78 00 00 00 80 74 00 00 00 32 00 00 00 31 48 87 78 00 00 00 80 76 00 00 00 32 00 00 00 33 48 87 78 00 00 00 80 78 00 00 00 33 00 00 00 2D 48 87 78 00 00 00 80 7A 00 00 00 33 00 00 00 2F 48 87 78 00 00 00 80 7C 00 00 00 34 00 00 00 29 48 87 78 00 00 00 80 7E 00 00 00 34 00 00 00 2B 48 87 78 00 00 00 80 80 00 00 00 35 00 00 00 25 48 87 78 00 00 00 80 82 00 00 00 35 00 00 00 27 48 87 78 00 00 00 80 6C 00 00 00 30 00 00 00 21 48 87 78 00 00 00 80 86 00 00 00 31 00 00 00 23 48 87 78 00 00 00 80 88 00 00 00 31 00 00 00 1D 48 87 78 00 00 00 80 8A 00 00 00 32 00 00 00 1F 48 87 78 00 00 00 80 8C 00 00 00 32 00 00 00 19 48 87 78 00 00 00 80 8E 00 00 00 33 00 00 00 1B 48 87 78 00 00 00 80 90 00 00 00 33 00 00 00 15 48 87 78 00 00 00 80 92 00 00 00 34 00 00 00 17 48 87 78 00 00 00 80 94 00 00 00 34 00 00 00 11 48 87 78 00 00 00 80 96 00 00 00 35 00 00 00 13 48 87 78 00 00 00 80 98 00 00 00 35 00 00 00 0D 48 87 78 00 00 00 80 9A 00 00 00 36 00 00 00 0F 48 87 78 00 00 00 00 00 00 00 00 02 00 00 00 01 FA AE 4F 00 2C 00 C1 4D 4C 00 C2 4D 4C 00 C3 4D 4C 00 E4 4B 4C 00 E5 4B 4C 00 E6 4B 4C 00 41 4B 4C 00 42 4B 4C 00 43 4B 4C 00 44 4B 4C 00 45 4B 4C 00 48 4B 4C 00 49 4B 4C 00 4A 4B 4C 00 4B 4B 4C 00 4D 4B 4C 00 50 4B 4C 00 54 4B 4C 00 56 4B 4C 00 57 4B 4C 00 58 4B 4C 00 5B 4B 4C 00 6B 4B 4C 00 6E 4B 4C 00 82 4B 4C 00 88 4B 4C 00 91 4B 4C 00 0B 4C 4C 00 0C 4C 4C 00 0D 4C 4C 00 7D 4C 4C 00 80 4C 4C 00 81 4C 4C 00 82 4C 4C 00 96 4C 4C 00 97 4C 4C 00 98 4C 4C 00 9F 4B 4C 00 F6 4B 4C 00 F7 4B 4C 00 F8 4B 4C 00 53 4C 4C 00 54 4C 4C 00 55 4C 4C 00 01 EB AE 4F 00 28 00 B0 11 10 00 19 60 10 00 B1 11 10 00 1A 60 10 00 B2 11 10 00 1B 60 10 00 B3 11 10 00 1C 60 10 00 35 55 0F 00 AB 11 10 00 3F FB 19 00 7E 53 0F 00 B8 E7 0F 00 46 35 10 00 2D 68 4F 00 CA 4A 0F 00 59 71 0F 00 60 98 0F 00 D7 D0 10 00 F6 82 10 00 19 5C 10 00 58 BF 0F 00 5A 95 4E 00 5B 95 4E 00 31 9D 4E 00 32 9D 4E 00 F5 A0 4E 00 39 99 4E 00 64 9D 4E 00 65 9D 4E 00 66 9D 4E 00 67 9D 4E 00 68 9D 4E 00 69 9D 4E 00 6A 9D 4E 00 6B 9D 4E 00 20 74 4D 00 04 3F 4D 00 F1 7B 4D 00 80 64 4D 00 00 00 00 00 00 00");

	pSocket->SendPacket(&oPacket);

	auto deleter = [](User* p) { FreeObj(p); };
	std::shared_ptr<User> pUser{ AllocObjCtor( User )((ClientSocket*)pSocket, iPacket), deleter };
	WvsBase::GetInstance<WvsShop>()->OnUserConnected(pUser);

}

void Center::OnCenterMigrateOutResult(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsShop>()->GetSocket(nClientSocketID);
	OutPacket oPacket;
	bool bSuccess = iPacket->Decode1() == 1 ? true : false;
	if (bSuccess)
	{
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnTransferChannel);

		// 7 = Header(2) + nClientSocketID(4) + bSuccess(1)
		oPacket.EncodeBuffer(iPacket->GetPacket() + 7, iPacket->GetPacketSize() - 7);
	}
	pSocket->SendPacket(&oPacket);
}
