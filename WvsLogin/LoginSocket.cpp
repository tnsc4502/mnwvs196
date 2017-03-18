#include "LoginSocket.h"
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

LoginSocket::LoginSocket(asio::io_service& serverService)
	: SocketBase(serverService)
{
}


LoginSocket::~LoginSocket()
{
}

void LoginSocket::OnPacket(InPacket *iPacket)
{
	printf("[ServerBase::OnPacket]");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	printf("nType = %d\n", nType);
	switch (nType)
	{
	case 0xA5:
		OutPacket oPacket;
		oPacket.Encode2(0x2F);
		oPacket.Encode4(0);
		SendPacket(&oPacket);
		break;
	}
}

void LoginSocket::OnClosed()
{

}