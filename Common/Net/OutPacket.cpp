#include "OutPacket.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include "InPacket.h"

OutPacket::OutPacket()
	: aBuff((unsigned char*)stMemoryPoolMan->AllocateArray(DEFAULT_BUFF_SIZE)),
	  nBuffSize(DEFAULT_BUFF_SIZE),
	  nPacketSize(4)
{
}

OutPacket::OutPacket(short opcode) 
	: aBuff((unsigned char*)stMemoryPoolMan->AllocateArray(DEFAULT_BUFF_SIZE)),
	  nBuffSize(DEFAULT_BUFF_SIZE),
	  nPacketSize(4)
{
}

OutPacket::~OutPacket()
{
}

void OutPacket::Encode1(char value)
{
	if (nPacketSize + sizeof(value) >= nBuffSize)
	{
		decltype(aBuff) newBuff = (decltype(aBuff))stMemoryPoolMan->AllocateArray(nBuffSize * 2);
		memcpy(newBuff, aBuff, nBuffSize);
		nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(aBuff);
		aBuff = newBuff;
	}
	*(decltype(value)*)(aBuff + nPacketSize) = value;
	nPacketSize += sizeof(value);
}

void OutPacket::Encode2(short value)
{
	if (nPacketSize + sizeof(value) >= nBuffSize)
	{
		decltype(aBuff) newBuff = (decltype(aBuff))stMemoryPoolMan->AllocateArray(nBuffSize * 2);
		memcpy(newBuff, aBuff, nBuffSize);
		nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(aBuff);
		aBuff = newBuff;
	}
	*(decltype(value)*)(aBuff + nPacketSize) = value;
	nPacketSize += sizeof(value);
}

void OutPacket::Encode4(int value)
{
	if (nPacketSize + sizeof(value) >= nBuffSize)
	{
		decltype(aBuff) newBuff = (decltype(aBuff))stMemoryPoolMan->AllocateArray(nBuffSize * 2);
		memcpy(newBuff, aBuff, nBuffSize);
		nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(aBuff);
		aBuff = newBuff;
	}
	*(decltype(value)*)(aBuff + nPacketSize) = value;
	nPacketSize += sizeof(value);
}

void OutPacket::Encode8(long long int value)
{
	if (nPacketSize + sizeof(value) >= nBuffSize)
	{
		decltype(aBuff) newBuff = (decltype(aBuff))stMemoryPoolMan->AllocateArray(nBuffSize * 2);
		memcpy(newBuff, aBuff, nBuffSize);
		nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(aBuff);
		aBuff = newBuff;
	}
	*(decltype(value)*)(aBuff + nPacketSize) = value;
	nPacketSize += sizeof(value);
}

void OutPacket::EncodeBuffer(unsigned char *buff, int size)
{
	if (nPacketSize + size >= nBuffSize)
	{
		decltype(aBuff) newBuff = (decltype(aBuff))stMemoryPoolMan->AllocateArray(nBuffSize * 2);
		memcpy(newBuff, aBuff, nBuffSize);
		nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(aBuff);
		aBuff = newBuff;
	}
	memcpy(aBuff + nPacketSize, buff, size);
	nPacketSize += size;
}

void OutPacket::EncodeStr(const std::string &str)
{
	Encode2((short)str.size());
	if (nPacketSize + str.size() >= nBuffSize)
	{
		decltype(aBuff) newBuff = (decltype(aBuff))stMemoryPoolMan->AllocateArray(nBuffSize * 2);
		memcpy(newBuff, aBuff, nBuffSize);
		nBuffSize *= 2;
		stMemoryPoolMan->DestructArray(aBuff);
		aBuff = newBuff;
	}
	memcpy(aBuff + nPacketSize, str.c_str(), str.size());
	nPacketSize += (unsigned int)str.size();
}

void OutPacket::Release()
{
	stMemoryPoolMan->DestructArray(aBuff);
}

void OutPacket::CopyFromTransferedPacket(InPacket *oPacket)
{
}
