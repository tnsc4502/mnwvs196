#pragma once
#include <vector>

class InPacket;
class OutPacket;

/*
用來保留User轉換頻道或者進入商城的時候，所需要暫存的資訊
*/
class UserTransferStatus
{
	int m_nChannelID;
	struct TransferTemporaryStatus
	{
		int nSkillID, nSLV, tTime;

		void Encode(OutPacket *oPacket) const;
		void Decode(InPacket *iPacket);
	};
	std::vector<TransferTemporaryStatus> m_aTS;

public:
	UserTransferStatus();
	~UserTransferStatus();

	void Decode(InPacket* iPacket);
	void Encode(OutPacket* oPacket) const;
};

