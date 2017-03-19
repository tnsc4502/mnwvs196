#include "ClientSocket.h"
#include "Net\InPacket.h"
#include "Net\OutPacket.h"

ClientSocket::ClientSocket(asio::io_service& serverService)
	: SocketBase(serverService)
{
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::OnClosed()
{
}

void ClientSocket::OnPacket(InPacket *iPacket)
{

}
