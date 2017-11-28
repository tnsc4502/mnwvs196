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
	printf("[WvsLogin][LoginSocket::OnPacket]封包接收：");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();

	switch (nType)
	{
	case ClientPacketFlag::ClientRequestStart:
		OnClientRequestStart();
		break;
	case ClientPacketFlag::ClientLoginBackgroundRequest:
		OnLoginBackgroundRequest();
		break;
	case ClientPacketFlag::ClientCheckPasswordRequest:
		OnCheckPasswordRequst(iPacket);
		break;
	case ClientPacketFlag::ClientSelectWorld:
		OnClientSelectWorld(iPacket);
		break;
	case ClientPacketFlag::ClientSecondPasswordCheck:
		OnClientSecondPasswdCheck();
		break;
	case ClientPacketFlag::ClientCheckDuplicatedID:
		OnClientCheckDuplicatedID(iPacket);
		break;
	case ClientPacketFlag::ClientCreateNewCharactar:
		OnClientCreateNewCharacter(iPacket);
		break;
	case ClientPacketFlag::ClientSelectCharacter:
		OnClientSelectCharacter(iPacket);
		break;
	case ClientPacketFlag::ClientRequestServerList:
		SendWorldInformation();
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
	oPacket.Encode2(LoginPacketFlag::ClientLoginBackgroundResponse);
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
	oPacket.Encode2(LoginPacketFlag::ClientChannelBackgroundResponse);
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
			OutPacket oPacket;
			oPacket.Encode2(LoginPacketFlag::WorldInformationResponse);
			oPacket.Encode1(pCenter->GetWorldInfo().nWorldID);
			oPacket.EncodeStr(pCenter->GetWorldInfo().strWorldDesc);
			oPacket.Encode1(pCenter->GetWorldInfo().nEventType);
			oPacket.EncodeStr(pCenter->GetWorldInfo().strEventDesc);
			oPacket.Encode2(0x64);
			oPacket.Encode2(0x64);
			oPacket.Encode1(pCenter->GetWorldInfo().nGameCount);
			for (int i = 1; i <= pCenter->GetWorldInfo().nGameCount; ++i)
			{
				oPacket.EncodeStr("Channel " + std::to_string(i));
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

void LoginSocket::OnClientSelectWorld(InPacket *iPacket)
{
	bool isRelogin = iPacket->Decode1() == 1;
	int worldIndex = iPacket->Decode1();
	int channelIndex = iPacket->Decode1();
	if (WvsBase::GetInstance<WvsLogin>()->GetCenter(worldIndex)->IsConnected())
	{
		OutPacket oPacket;
		oPacket.Encode2(LoginPacketFlag::RequestCharacterList);
		oPacket.Encode4(GetSocketID());
		oPacket.Encode4(mLoginData.nAccountID);
		oPacket.Encode1(channelIndex);
		WvsBase::GetInstance<WvsLogin>()->GetCenter(worldIndex)->SendPacket(&oPacket);
		nChannelID = channelIndex;
		nWorldID = worldIndex;
	}
	else
		printf("[WvsLogin][LoginSocket::OnClientSelectWorld][錯誤]客戶端嘗試連線至不存在的Center Server。\n");
}

void LoginSocket::OnClientSecondPasswdCheck()
{
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::ClientSecondPasswordResult);
	oPacket.Encode1(0x03);
	oPacket.Encode1(0x00);
	SendPacket(&oPacket);
}

void LoginSocket::OnClientCheckDuplicatedID(InPacket *iPacket)
{
	std::string strName = iPacket->DecodeStr();

	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::ClientCheckDuplicatedIDResult);
	oPacket.EncodeStr(strName);
	oPacket.Encode1(0); //SUCCESS

	SendPacket(&oPacket);
}

void LoginSocket::OnClientCreateNewCharacter(InPacket *iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::RequestCreateNewCharacter);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(mLoginData.nAccountID);
	oPacket.EncodeBuffer(iPacket->GetPacket() + 2, iPacket->GetPacketSize() - 2); //SKIP OPCODE
	WvsBase::GetInstance<WvsLogin>()->GetCenter(nWorldID)->SendPacket(&oPacket);
}

void LoginSocket::OnClientSelectCharacter(InPacket *iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(LoginPacketFlag::RequestGameServerInfo);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(nWorldID);
	oPacket.Encode4(nChannelID);
	oPacket.EncodeBuffer(iPacket->GetPacket() + 2, iPacket->GetPacketSize() - 2);
	WvsBase::GetInstance<WvsLogin>()->GetCenter(nWorldID)->SendPacket(&oPacket);
}