#include "WvsCenter.h"

#include "Net\PacketFlags\CenterPacketFlags.hpp"
#include "Net\OutPacket.h"

#include "Constants\ServerConstants.hpp"

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
		for (int i = 0; i < 30; ++i)
		{
			if (aChannel[i].GetGameServer().get())
			{
				for (int j = i + 1; j < 30; ++j)
					aChannel[j - 1] = aChannel[j];
				--i;
			}
		}
		--nConnectedChannel;
		NotifyWorldChanged();
	}
}

void WvsCenter::NotifyWorldChanged()
{
	auto& socketList = WvsBase::GetInstance<WvsCenter>()->GetSocketList();
	for (const auto& socket : socketList)
	{
		printf("Server Type = %d\n", socket.second->GetServerType());
		if (socket.second->GetServerType() != ServerConstants::SVR_GAME)
		{
			OutPacket oPacket;
			oPacket.Encode2(CenterPacketFlag::CenterStatChanged);
			oPacket.Encode2(WvsBase::GetInstance<WvsCenter>()->GetChannelCount());
			socket.second->SendPacket(&oPacket);
		}
	}
}