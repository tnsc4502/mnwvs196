#pragma once
#include "..\WvsLib\Net\SocketBase.h"

class LoginSocket :
	public SocketBase
{
private:
	struct LoginData
	{
		int nAccountID;
		std::string strAccountName;
		unsigned char nLoginState;
	} mLoginData;

	void OnClosed();
	int nWorldID, nChannelID;

public:
	LoginSocket(asio::io_service& serverService);
	~LoginSocket();

	void OnPacket(InPacket *iPacket);

	void OnClientRequestStart();
	void OnLoginBackgroundRequest();
	void OnCheckPasswordRequst(InPacket *iPacket);
	void SendChannelBackground();
	void SendWorldInformation();
	void OnClientSelectWorld(InPacket *iPacket);
	void OnClientSecondPasswdCheck();
	void OnClientCheckDuplicatedID(InPacket *iPacket);
	void OnClientCreateNewCharacter(InPacket *iPacket);
	void OnClientSelectCharacter(InPacket *iPacket);
};

