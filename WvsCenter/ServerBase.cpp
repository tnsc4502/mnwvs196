#include "ServerBase.h"
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

ServerBase::ServerBase(asio::io_service& serverService)
	: SocketBase(serverService)
{
}

ServerBase::~ServerBase()
{
}

void ServerBase::OnClosed()
{

}

void ServerBase::OnPacket(InPacket *iPacket)
{
	printf("[ServerBase::OnPacket]");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	printf("nType = %d\n", nType);
	switch (nType)
	{
	case 0xFFFF:
		auto serverBaseType = iPacket->Decode1();
		if (serverBaseType == 1)
		{
			printf("Login Server Registered!\n");
			OutPacket oPacket;
			oPacket.Encode2(0xFF);
			oPacket.Encode4(1); //Success;
			SendPacket(&oPacket);
		}
		break;
	}
}