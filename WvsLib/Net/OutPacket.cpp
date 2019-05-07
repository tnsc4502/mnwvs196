#include "OutPacket.h"
#include "InPacket.h"
#include "..\Logger\WvsLogger.h"
#include "..\Memory\MemoryPoolMan.hpp"

void OutPacket::ExtendSize(int nExtendRate = 2)
{
	decltype(m_pSharedPacket->aBuff) newBuff = AllocArray(unsigned char, (m_pSharedPacket->nBuffSize * nExtendRate));
	memcpy(newBuff, m_pSharedPacket->aBuff, m_pSharedPacket->nBuffSize);
	FreeArray(m_pSharedPacket->aBuff, m_pSharedPacket->nBuffSize);
	m_pSharedPacket->nBuffSize *= nExtendRate;
	m_pSharedPacket->aBuff = newBuff;
}

OutPacket::OutPacket()
{
	m_pSharedPacket = AllocObj(SharedPacket);
	(*((long long int*)m_pSharedPacket->aBuff)) = (long long int)(m_pSharedPacket);
}

OutPacket::~OutPacket()
{
	DecRefCount();
}

void OutPacket::Encode1(char value)
{
	if (m_pSharedPacket->nPacketSize + sizeof(value) >= m_pSharedPacket->nBuffSize)
		ExtendSize();
	*(decltype(value)*)(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize) = value;
	m_pSharedPacket->nPacketSize += sizeof(value);
}

void OutPacket::Encode2(short value)
{
	if (m_pSharedPacket->nPacketSize + sizeof(value) >= m_pSharedPacket->nBuffSize)
		ExtendSize();
	*(decltype(value)*)(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize) = value;
	m_pSharedPacket->nPacketSize += sizeof(value);
}

void OutPacket::Encode4(int value)
{
	if (m_pSharedPacket->nPacketSize + sizeof(value) >= m_pSharedPacket->nBuffSize)
		ExtendSize();
	*(decltype(value)*)(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize) = value;
	m_pSharedPacket->nPacketSize += sizeof(value);
}

void OutPacket::Encode8(long long int value)
{
	if (m_pSharedPacket->nPacketSize + sizeof(value) >= m_pSharedPacket->nBuffSize)
		ExtendSize();
	*(decltype(value)*)(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize) = value;
	m_pSharedPacket->nPacketSize += sizeof(value);
}

void OutPacket::EncodeBuffer(unsigned char *buff, int nSize, int nZero)
{
	if (m_pSharedPacket->nPacketSize + nSize + nZero >= m_pSharedPacket->nBuffSize)
		ExtendSize((int)std::ceil((m_pSharedPacket->nPacketSize + nSize + nZero) / m_pSharedPacket->nBuffSize) + 1);
	if (buff == nullptr) 
	{
		int nEncode4Count = nSize / 4;
		int remain = nSize % 4;
		for (int i = 0; i < nEncode4Count; ++i)
			Encode4(0);
		for (int i = 0; i < remain; ++i)
			Encode1(0);
	}
	else 
	{
		memcpy(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize, buff, nSize);
		m_pSharedPacket->nPacketSize += nSize;
		if (nZero > 0)
			for (int i = 0; i < nZero; ++i)
				Encode1(0);
	}
}

void OutPacket::EncodeStr(const std::string &str)
{
	Encode2((short)str.size());
	if (m_pSharedPacket->nPacketSize + str.size() >= m_pSharedPacket->nBuffSize)
		ExtendSize();
	memcpy(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize, str.c_str(), str.size());
	m_pSharedPacket->nPacketSize += (unsigned int)str.size();
}

void OutPacket::Release()
{
	FreeArray(m_pSharedPacket->aBuff, m_pSharedPacket->nBuffSize);
	//delete[] m_pSharedPacket->aBuff;
}

void OutPacket::Reset()
{
	m_pSharedPacket->nPacketSize = INITIAL_WRITE_INDEX;
}

void OutPacket::IncRefCount()
{
	m_pSharedPacket->IncRefCount();
}

void OutPacket::DecRefCount()
{
	m_pSharedPacket->DecRefCount();
}

void OutPacket::Print()
{
	WvsLogger::LogRaw("OutPacket«Ê¥]¤º®e¡G");
	for (unsigned int i = 0; i < m_pSharedPacket->nPacketSize; ++i)
		WvsLogger::LogFormat("0x%02X ", (int)m_pSharedPacket->aBuff[i]);
	WvsLogger::LogRaw("\n");
}

OutPacket::SharedPacket * OutPacket::GetSharedPacket()
{
	return m_pSharedPacket;
}

void OutPacket::CopyFromTransferedPacket(InPacket *oPacket)
{
}

void OutPacket::EncodeHexString(const std::string& str)
{
	auto cStr = str.c_str();
	int d, n;
	while (sscanf_s(cStr, "%X%n", &d, &n) == 1)
	{
		cStr += n;
		Encode1((char)d);
	}
}

OutPacket::SharedPacket::SharedPacket()
	: aBuff(AllocArray(unsigned char, DEFAULT_BUFF_SIZE)),
	nBuffSize(DEFAULT_BUFF_SIZE),
	nPacketSize(INITIAL_WRITE_INDEX),
	nRefCount(1)
{
}

OutPacket::SharedPacket::~SharedPacket()
{
}

void OutPacket::SharedPacket::ToggleBroadcasting()
{
	m_bBroadcasting = true;
}

bool OutPacket::SharedPacket::IsBroadcasting() const
{
	return m_bBroadcasting;
}

void OutPacket::SharedPacket::IncRefCount()
{
	++nRefCount;
}

void OutPacket::SharedPacket::DecRefCount()
{
	if (--nRefCount <= 0)
	{
		FreeArray(aBuff, nBuffSize);
		//delete[] aBuff;
		//MSMemoryPoolMan::GetInstance()->DestructArray(aBuff);
		//delete this;
		FreeObj(this);
	}
}
