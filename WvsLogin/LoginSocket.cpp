#include "LoginSocket.h"
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

#include "Net\PacketFlags\LoginPacketFlags.hpp"
#include "Net\PacketFlags\ClientPacketFlags.hpp"

#include "WvsLoginConstants.hpp"
#include "WvsLogin.h"

LoginSocket::LoginSocket(asio::io_service& serverService)
	: SocketBase(serverService)
{
}

LoginSocket::~LoginSocket()
{
}

void LoginSocket::OnPacket(InPacket *iPacket)
{
	printf("[LoginSocket::OnPacket]");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();

	switch (nType)
	{
	case ClientPacketFlag::ClientRequestStart:
		OnClientRequestStart();
		break;
	case ClientPacketFlag::LoginBackgroundRequest:
		OnLoginBackgroundRequest();
		break;
	case ClientPacketFlag::CheckPasswordRequest:
		OnCheckPasswordRequst(iPacket);
		break;
	}
}

void LoginSocket::OnClosed()
{

}

void LoginSocket::OnClientRequestStart()
{
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::ClientStartResponse);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

void LoginSocket::OnLoginBackgroundRequest()
{
	static std::string backgrounds[] = { "MapLogin", "MapLogin1", "MapLogin2" };
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::LoginBackgroundResponse);
	oPacket.EncodeStr(backgrounds[rand() % (sizeof(backgrounds) / sizeof(backgrounds[0]))]);
	oPacket.Encode4(0);
	oPacket.Encode1(1);
	SendPacket(&oPacket);
}

void LoginSocket::OnCheckPasswordRequst(InPacket *iPacket)
{
	iPacket->DecodeBuffer(nullptr, 22);
	auto sID = iPacket->DecodeStr();
	auto sPasswd = iPacket->DecodeStr();

	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::CheckPasswordResponse);
	oPacket.Encode1(0);
	oPacket.EncodeStr(sID);
	oPacket.Encode4(1); //Account ID
	oPacket.Encode1(0); //Gender
	oPacket.Encode1(0); //GM
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	oPacket.Encode4(0x22);
	oPacket.Encode1(0x03);
	oPacket.Encode1(0);
	oPacket.Encode8(0);
	oPacket.Encode1(0);
	oPacket.Encode8(0);
	oPacket.Encode1(0);
	oPacket.EncodeStr(sID);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(1);
	oPacket.Encode1(0xC2);
	for (int i = 1; i <= 23; ++i)
	{
		oPacket.Encode1(0x01);
		oPacket.Encode2(0x01);;
	}
	oPacket.Encode1(0);
	oPacket.Encode4(-1);
	oPacket.Encode1(0);

	SendPacket(&oPacket);
	SendWorldInformation();
}

void LoginSocket::SendChannelBackground()
{
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::ChannelBackgroundResponse);
	oPacket.Encode1(0);
}

void LoginSocket::SendWorldInformation()
{
	//SendChannelBackground();

	int numCenter = sizeof(WvsLoginConstants::CenterServerList) / sizeof(WvsLoginConstants::CenterServerList[0]);
	for (int i = 0; i < numCenter; ++i)
	{
		auto& pCenter = WvsBase::GetInstance<WvsLogin>()->GetCenter(i);
		if (pCenter->IsConnected())
		{
			printf("World\n");
			OutPacket oPacket;
			oPacket.Encode2(LoginPacketFlag::WorldInformationResponse);
			oPacket.Encode1(pCenter->GetWorldInfo().nWorldID);
			oPacket.EncodeStr("Test");
			oPacket.Encode1(1);
			oPacket.EncodeStr("Test!");
			oPacket.Encode2(0x64);
			oPacket.Encode2(0x64);
			oPacket.Encode1(pCenter->GetWorldInfo().nGameCount);
			for (int i = 1; i <= pCenter->GetWorldInfo().nGameCount; ++i)
			{
				oPacket.EncodeStr("C" + std::to_string(i));
				oPacket.Encode4(1);
				oPacket.Encode1(pCenter->GetWorldInfo().nWorldID);
				oPacket.Encode2(i - 1);
			}
			oPacket.Encode4(0);
			oPacket.Encode2(0);
			oPacket.Encode1(0);
			SendPacket(&oPacket);
		}
	}
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::WorldInformationResponse);
	oPacket.Encode4(0xFF);
	SendPacket(&oPacket);
}