#pragma once

class User;
class InPacket;

class QWUInventory
{
public:
	QWUInventory();
	~QWUInventory();
	static bool ChangeSlotPosition(User* pUser, int bOnExclRequest, int nTI, int nPOS1, int nPOS2, int nCount, int tRequestTime);
	static void OnChangeSlotPositionRequest(User* pUser, InPacket* iPacket);
};

