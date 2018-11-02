#include "LoginSocket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"

#include "..\WvsLib\Logger\WvsLogger.h"

#include "..\WvsLib\Common\WvsLoginConstants.hpp"
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
	WvsLogger::LogRaw("[WvsLogin][LoginSocket::OnPacket]封包接收：");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();

	switch (nType)
	{
	case LoginRecvPacketFlag::Client_ClientRequestStart:
		OnClientRequestStart();
		break;
	case LoginRecvPacketFlag::Client_ClientLoginBackgroundRequest:
		OnLoginBackgroundRequest();
		break;
	case LoginRecvPacketFlag::Client_ClientCheckPasswordRequest:
		OnCheckPasswordRequst(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientSelectWorld:
		OnClientSelectWorld(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientSecondPasswordCheck:
		OnClientSecondPasswdCheck();
		break;
	case LoginRecvPacketFlag::Client_ClientCheckDuplicatedID:
		OnClientCheckDuplicatedID(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientCreateNewCharactar:
		OnClientCreateNewCharacter(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientSelectCharacter:
		OnClientSelectCharacter(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientRequestServerList:
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
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientStartResponse);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

void LoginSocket::OnLoginBackgroundRequest()
{
	static std::string backgrounds[] = { "MapLogin", "MapLogin1", "MapLogin2" };
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientLoginBackgroundResponse);
	oPacket.EncodeStr(backgrounds[rand() % (sizeof(backgrounds) / sizeof(backgrounds[0]))]);
	oPacket.Encode4(0);
	oPacket.Encode1(1);
	SendPacket(&oPacket);
}

void LoginSocket::OnCheckPasswordRequst(InPacket *iPacket)
{
	mLoginData.nAccountID = 1;
	iPacket->DecodeBuffer(nullptr, 22);
	auto sID = iPacket->DecodeStr();
	auto sPasswd = iPacket->DecodeStr();

	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_CheckPasswordResponse);
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
	oPacket.Encode1((char)0xC2);
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
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientChannelBackgroundResponse);
	oPacket.Encode1(0);
	SendPacket(&oPacket);
}

void LoginSocket::SendWorldInformation()
{
	SendChannelBackground();

	int nCenterCount = WvsBase::GetInstance<WvsLogin>()->GetCenterCount();
	for (int i = 0; i < nCenterCount; ++i)
	{
		auto& pCenter = WvsBase::GetInstance<WvsLogin>()->GetCenter(i);
		if (pCenter && pCenter->CheckSocketStatus(SocketBase::SocketStatus::eConnected))
		{
			OutPacket oPacket;
			oPacket.Encode2(LoginSendPacketFlag::Client_WorldInformationResponse);
			oPacket.Encode1(pCenter->GetWorldInfo().nWorldID);
			oPacket.EncodeStr(pCenter->GetWorldInfo().strWorldDesc);
			oPacket.Encode1(pCenter->GetWorldInfo().nEventType);
			oPacket.EncodeStr(pCenter->GetWorldInfo().strEventDesc);
			oPacket.Encode2(0x64);
			oPacket.Encode2(0x64);
			int nMaxChannelCount = pCenter->GetWorldInfo().nGameCount;
			for (int i = 0; i < 30; ++i)
				if (pCenter->GetWorldInfo().m_aChannelStatus[i] == 1 && i + 1 > nMaxChannelCount)
					nMaxChannelCount = i + 1;
			oPacket.Encode1(nMaxChannelCount);
			for (int i = 1; i <= nMaxChannelCount; ++i)
			{
				oPacket.EncodeStr("Channel " + std::to_string(i));
				oPacket.Encode4(pCenter->GetWorldInfo().m_aChannelStatus[i - 1] == 0 ? 100000 : 1);
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
	oPacket.Encode2(LoginSendPacketFlag::Client_WorldInformationResponse);
	oPacket.Encode4(0xFF);
	SendPacket(&oPacket);

	OutPacket oPacket2;
	oPacket2.Encode2(0x03);
	oPacket2.Encode1(1);
	oPacket2.Encode4(0);
	oPacket2.EncodeStr("Hello~");
	SendPacket(&oPacket2);
}

void LoginSocket::OnClientSelectWorld(InPacket *iPacket)
{
	bool bIsRelogin = iPacket->Decode1() == 1;
	int nWorldIndex = iPacket->Decode1();
	int nChannelIndex = iPacket->Decode1();
	if (WvsBase::GetInstance<WvsLogin>()->GetCenter(nWorldIndex)->CheckSocketStatus(SocketBase::SocketStatus::eConnected))
	{
		OutPacket oPacket;
		oPacket.Encode2(LoginSendPacketFlag::Center_RequestCharacterList);
		oPacket.Encode4(GetSocketID());
		oPacket.Encode4(mLoginData.nAccountID);
		oPacket.Encode1(nChannelIndex);
		WvsBase::GetInstance<WvsLogin>()->GetCenter(nWorldIndex)->SendPacket(&oPacket);
		nChannelID = nChannelIndex;
		nWorldID = nWorldIndex;
	}
	else
		WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsLogin][LoginSocket::OnClientSelectWorld][錯誤]客戶端嘗試連線至不存在的Center Server。\n");
}

void LoginSocket::OnClientSecondPasswdCheck()
{
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientSecondPasswordResult);
	oPacket.Encode1(0x03);
	oPacket.Encode1(0x00);
	SendPacket(&oPacket);
}

void LoginSocket::OnClientCheckDuplicatedID(InPacket *iPacket)
{
	std::string strName = iPacket->DecodeStr();

	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientCheckDuplicatedIDResult);
	oPacket.EncodeStr(strName);
	oPacket.Encode1(0); //SUCCESS

	SendPacket(&oPacket);
}

void LoginSocket::OnClientCreateNewCharacter(InPacket *iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RequestCreateNewCharacter);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(mLoginData.nAccountID);
	oPacket.EncodeBuffer(iPacket->GetPacket() + 2, iPacket->GetPacketSize() - 2); //SKIP OPCODE
	WvsBase::GetInstance<WvsLogin>()->GetCenter(nWorldID)->SendPacket(&oPacket);
}

void LoginSocket::OnClientSelectCharacter(InPacket *iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RequestGameServerInfo);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(nWorldID);
	oPacket.Encode4(nChannelID);
	oPacket.EncodeBuffer(iPacket->GetPacket() + 2, iPacket->GetPacketSize() - 2);
	WvsBase::GetInstance<WvsLogin>()->GetCenter(nWorldID)->SendPacket(&oPacket);
}