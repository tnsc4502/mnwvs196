#include "WvsCenter.h"

#include "Net\PacketFlags\CenterPacketFlags.hpp"
#include "Net\OutPacket.h"

#include "Constants\ServerConstants.hpp"
#include "Constants\ConfigLoader.hpp"

WvsCenter::WvsCenter()
{
}

WvsCenter::~WvsCenter()
{
}

void WvsCenter::OnNotifySocketDisconnected(SocketBase *pSocket)
{
	printf("On Notify Socket Disconnected\n");
	if (pSocket->GetServerType() == ServerConstants::SVR_GAME)
	{
		for (int i = 0; i < nConnectedChannel - 1; ++i)
		{
			if (aChannel[i].GetGameServer().get() == pSocket)
			{
				for (int j = i + 1; j < nConnectedChannel; ++j)
					aChannel[j - 1] = aChannel[j];
				--i;
			}
		}
		--nConnectedChannel;
		NotifyWorldChanged();
	}
}

void WvsCenter::Init()
{
	mWorldInfo.nEventType = ConfigLoader::GetInstance()->IntValue("EventType");
	mWorldInfo.nWorldID = ConfigLoader::GetInstance()->IntValue("WorldID");
	mWorldInfo.strEventDesc = ConfigLoader::GetInstance()->StrValue("EventDesc");
	mWorldInfo.strWorldDesc = ConfigLoader::GetInstance()->StrValue("WorldDesc");
}

void WvsCenter::NotifyWorldChanged()
{
	auto& socketList = WvsBase::GetInstance<WvsCenter>()->GetSocketList();
	for (const auto& socket : socketList)
	{
		if (socket.second->GetServerType() != ServerConstants::SVR_GAME)
		{
			printf("On Notify World Changed\n");
			OutPacket oPacket;
			oPacket.Encode2(CenterPacketFlag::CenterStatChanged);
			oPacket.Encode2(WvsBase::GetInstance<WvsCenter>()->GetChannelCount());
			socket.second->SendPacket(&oPacket);
		}
	}
}