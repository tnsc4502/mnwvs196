#include "UserTransferStatus.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

UserTransferStatus::UserTransferStatus()
{
}


UserTransferStatus::~UserTransferStatus()
{
}

void UserTransferStatus::Decode(InPacket * iPacket)
{
	m_nChannelID = iPacket->Decode4();
	//Decode Temporary Status
	int nCount = iPacket->Decode4();
	m_aTS.resize(nCount);
	for (int i = 0; i < nCount; ++i)
		m_aTS[i].Decode(iPacket);
}

void UserTransferStatus::Encode(OutPacket * oPacket) const
{
	oPacket->Encode4(m_nChannelID);
	//Encode Temporary Status
	oPacket->Encode4((int)m_aTS.size());
	for (auto& ts : m_aTS)
		ts.Encode(oPacket);
}

void UserTransferStatus::TransferTemporaryStatus::Encode(OutPacket * oPacket) const
{
	oPacket->Encode4(nSkillID);
	oPacket->Encode4(tTime);
	oPacket->Encode4(nSLV);
}

void UserTransferStatus::TransferTemporaryStatus::Decode(InPacket * iPacket)
{
	nSkillID = iPacket->Decode4();
	tTime = iPacket->Decode4();
	nSLV = iPacket->Decode4();
}
