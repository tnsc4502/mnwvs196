#include "OutPacket.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include "InPacket.h"

OutPacket::OutPacket()
{
	m_pSharedPacket = new SharedPacket;
	(*((long long int*)m_pSharedPacket->aBuff)) = (long long int)(m_pSharedPacket);
}

OutPacket::~OutPacket()
{
	DecRefCount();
}

void OutPacket::Encode1(char value)
{
	if (m_pSharedPacket->nPacketSize + sizeof(value) >= m_pSharedPacket->nBuffSize)
	{
		decltype(m_pSharedPacket->aBuff) newBuff = (decltype(m_pSharedPacket->aBuff))stMemoryPoolMan->AllocateArray(m_pSharedPacket->nBuffSize * 2);
		memcpy(newBuff, m_pSharedPacket->aBuff, m_pSharedPacket->nBuffSize);
		m_pSharedPacket->nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(m_pSharedPacket->aBuff);
		m_pSharedPacket->aBuff = newBuff;
	}
	*(decltype(value)*)(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize) = value;
	m_pSharedPacket->nPacketSize += sizeof(value);
}

void OutPacket::Encode2(short value)
{
	if (m_pSharedPacket->nPacketSize + sizeof(value) >= m_pSharedPacket->nBuffSize)
	{
		decltype(m_pSharedPacket->aBuff) newBuff = (decltype(m_pSharedPacket->aBuff))stMemoryPoolMan->AllocateArray(m_pSharedPacket->nBuffSize * 2);
		memcpy(newBuff, m_pSharedPacket->aBuff, m_pSharedPacket->nBuffSize);
		m_pSharedPacket->nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(m_pSharedPacket->aBuff);
		m_pSharedPacket->aBuff = newBuff;
	}
	*(decltype(value)*)(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize) = value;
	m_pSharedPacket->nPacketSize += sizeof(value);
}

void OutPacket::Encode4(int value)
{
	if (m_pSharedPacket->nPacketSize + sizeof(value) >= m_pSharedPacket->nBuffSize)
	{
		decltype(m_pSharedPacket->aBuff) newBuff = (decltype(m_pSharedPacket->aBuff))stMemoryPoolMan->AllocateArray(m_pSharedPacket->nBuffSize * 2);
		memcpy(newBuff, m_pSharedPacket->aBuff, m_pSharedPacket->nBuffSize);
		m_pSharedPacket->nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(m_pSharedPacket->aBuff);
		m_pSharedPacket->aBuff = newBuff;
	}
	*(decltype(value)*)(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize) = value;
	m_pSharedPacket->nPacketSize += sizeof(value);
}

void OutPacket::Encode8(long long int value)
{
	if (m_pSharedPacket->nPacketSize + sizeof(value) >= m_pSharedPacket->nBuffSize)
	{
		decltype(m_pSharedPacket->aBuff) newBuff = (decltype(m_pSharedPacket->aBuff))stMemoryPoolMan->AllocateArray(m_pSharedPacket->nBuffSize * 2);
		memcpy(newBuff, m_pSharedPacket->aBuff, m_pSharedPacket->nBuffSize);
		m_pSharedPacket->nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(m_pSharedPacket->aBuff);
		m_pSharedPacket->aBuff = newBuff;
	}
	*(decltype(value)*)(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize) = value;
	m_pSharedPacket->nPacketSize += sizeof(value);
}

void OutPacket::EncodeBuffer(unsigned char *buff, int size)
{
	if (m_pSharedPacket->nPacketSize + size >= m_pSharedPacket->nBuffSize)
	{
		decltype(m_pSharedPacket->aBuff) newBuff = (decltype(m_pSharedPacket->aBuff))stMemoryPoolMan->AllocateArray((m_pSharedPacket->nPacketSize + size) * 2);
		memcpy(newBuff, m_pSharedPacket->aBuff, m_pSharedPacket->nBuffSize);
		m_pSharedPacket->nBuffSize = (m_pSharedPacket->nPacketSize + size);
		stMemoryPoolMan->DestructArray(m_pSharedPacket->aBuff);
		m_pSharedPacket->aBuff = newBuff;
	}
	if (buff == nullptr) {
		int encode4Count = size / 4;
		int remain = size % 4;
		for (int i = 0; i < encode4Count; ++i)
			Encode4(0);
		for (int i = 0; i < remain; ++i)
			Encode1(0);
	}
	else {
		memcpy(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize, buff, size);
		m_pSharedPacket->nPacketSize += size;
	}
}

void OutPacket::EncodeStr(const std::string &str)
{
	Encode2((short)str.size());
	if (m_pSharedPacket->nPacketSize + str.size() >= m_pSharedPacket->nBuffSize)
	{
		decltype(m_pSharedPacket->aBuff) newBuff = (decltype(m_pSharedPacket->aBuff))stMemoryPoolMan->AllocateArray(m_pSharedPacket->nBuffSize * 2);
		memcpy(newBuff, m_pSharedPacket->aBuff, m_pSharedPacket->nBuffSize);
		m_pSharedPacket->nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(m_pSharedPacket->aBuff);
		m_pSharedPacket->aBuff = newBuff;
	}
	memcpy(m_pSharedPacket->aBuff + m_pSharedPacket->nPacketSize, str.c_str(), str.size());
	m_pSharedPacket->nPacketSize += (unsigned int)str.size();
}

void OutPacket::EncodeTime(int64_t timeValue)
{
	if (timeValue == -1)
		Encode8(150842304000000000L);
	else if (timeValue == -2)
		Encode8(94354848000000000L);
	else if (timeValue == -3)
		Encode8(150841440000000000L);
	else
		Encode8(timeValue * 10000L + 116444592000000000L);
}

void OutPacket::Release()
{
	stMemoryPoolMan->DestructArray(m_pSharedPacket->aBuff);
}

void OutPacket::IncRefCount()
{
	m_pSharedPacket->IncRefCount();
}

void OutPacket::DecRefCount()
{
	m_pSharedPacket->DecRefCount();
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
	while (sscanf(cStr, "%X%n", &d, &n) == 1)
	{
		cStr += n;
		Encode1((char)d);
	}
}

OutPacket::SharedPacket::SharedPacket()
	: aBuff((unsigned char*)stMemoryPoolMan->AllocateArray(DEFAULT_BUFF_SIZE)),
	nBuffSize(DEFAULT_BUFF_SIZE),
	nPacketSize(4 + 8),
	nRefCount(1)
{
}

OutPacket::SharedPacket::~SharedPacket()
{
}

void OutPacket::SharedPacket::IncRefCount()
{
	++nRefCount;
}

void OutPacket::SharedPacket::DecRefCount()
{
	if (--nRefCount <= 0)
	{
		stMemoryPoolMan->DestructArray(aBuff);
		delete this;
	}
}
