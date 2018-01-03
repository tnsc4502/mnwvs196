#include "WvsBase.h"
#include <mutex>

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
	printf("[WvsBase::CreateAcceptor]成功於Port %d建立Wvs伺服器程序。\n", nPort);
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
	printf("OnSocketDisconnected called!\n ");
	/*static std::mutex localLock;
	std::lock_guard<std::mutex> lock(localLock);*/
	auto findIter = aSocketList.find(pSocket->GetSocketID());
	if (findIter == aSocketList.end())
		return;
	aSocketList.erase(pSocket->GetSocketID());
	printf("[WvsBase::OnSocketDisconnected]移除遠端連線Socket實體，Socket ID = %d\n", pSocket->GetSocketID());
	pInstance->OnNotifySocketDisconnected(pSocket);
}

void WvsBase::OnNotifySocketDisconnected(SocketBase *pSocket)
{
	//WvsBase is an ADT, so won't become recursive call.
	this->OnNotifySocketDisconnected(pSocket);
}