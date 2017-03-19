#pragma once
#include "Net\asio.hpp"
#include <map>
#include "Net\SocketBase.h"
#include <functional>

class WvsBase
{
private:
	asio::io_service mIOService;
	asio::ip::tcp::acceptor *mAcceptor;
	static std::map<unsigned int, SocketBase*> aSocketList;

	template<typename SOCKET_TYPE>
	void OnAccepted(std::shared_ptr<SOCKET_TYPE> acceptedSocket, const std::error_code& ec)
	{
		std::cout << "New Server Base Connected!\n";
		OnSocketConnected((SocketBase*)(acceptedSocket.get()));
		acceptedSocket->SetDisconnectedNotifyFunc(OnSocketDisconnected);
		acceptedSocket->Init();
		BeginAccept<SOCKET_TYPE>();
	}

	static WvsBase* pInstance;

protected:
	WvsBase();
	virtual ~WvsBase();

public:

	template<typename WVS_TYPE>
	static WVS_TYPE *GetInstance()
	{
		static WVS_TYPE *sWvsWorld = new WVS_TYPE();
		pInstance = sWvsWorld;
		return sWvsWorld;
	}

	static WvsBase* GetRawInstance()
	{
		return pInstance;
	}

	static void OnSocketConnected(SocketBase *pSocket);
	static void OnSocketDisconnected(SocketBase *pSocket);

	virtual void OnNotifySocketDisconnected(SocketBase *pSocket) = 0;

	void CreateAcceptor(short nPort);

	//Template參數表示客戶端 (來源) 的Class型別
	template<typename SOCKET_TYPE>
	void BeginAccept()
	{
		std::shared_ptr<SOCKET_TYPE> session(new SOCKET_TYPE(GetIOService()));
		mAcceptor->async_accept(session->GetSocket(), std::bind(&WvsBase::OnAccepted<SOCKET_TYPE>, this, session, std::placeholders::_1));
	}

	virtual void Init();
	asio::io_service& GetIOService();

	std::map<unsigned int, SocketBase*>& GetSocketList()
	{
		return aSocketList;
	}

};

