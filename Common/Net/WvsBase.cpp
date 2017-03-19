#include "WvsBase.h"

std::map<unsigned int, SocketBase*> WvsBase::aSocketList;
WvsBase* WvsBase::pInstance;

WvsBase::WvsBase()
{
}

WvsBase::~WvsBase()
{
}

asio::io_service& WvsBase::GetIOService()
{
	return mIOService;
}

void WvsBase::Init()
{

}

void WvsBase::CreateAcceptor(short nPort)
{
	printf("[WvsBase::CreateAcceptor]Success Create WvsBase Service On Port %d\n", nPort);
	asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), nPort);
	mAcceptor = new asio::ip::tcp::acceptor(mIOService, endpoint);
}

void WvsBase::OnSocketConnected(SocketBase *pSocket)
{
	aSocketList.insert({ pSocket->GetSocketID(), pSocket });
	pSocket->SetDisconnectedNotifyFunc(WvsBase::OnSocketDisconnected);
}

void WvsBase::OnSocketDisconnected(SocketBase *pSocket)
{
	printf("[WvsBase::OnSocketDisconnected]Remove Socket From List, ID = %d\n", pSocket->GetSocketID());
	pInstance->OnNotifySocketDisconnected(pSocket);
	aSocketList.erase(pSocket->GetSocketID());
}

void WvsBase::OnNotifySocketDisconnected(SocketBase *pSocket)
{
	//WvsBase is an ADT, so won't become recursive call.
	this->OnNotifySocketDisconnected(pSocket);
}