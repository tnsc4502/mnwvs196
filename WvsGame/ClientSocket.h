#pragma once
#include "..\WvsLib\Net\SocketBase.h"

class User;

class ClientSocket :
	public SocketBase
{
	void OnClosed();
	User *pUser = nullptr;
public:
	ClientSocket(asio::io_service& serverService);
	~ClientSocket();

	void SetUser(User *_pUser);
	User* GetUser();

	void OnPacket(InPacket *iPacket);
	void OnMigrateIn(InPacket *iPacket);
};

