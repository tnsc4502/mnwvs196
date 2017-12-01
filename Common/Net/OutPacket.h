#pragma once
#include <string>

class InPacket;

class OutPacket
{
private:
	unsigned char* aBuff = nullptr;
	unsigned int nBuffSize = 0, nPacketSize = 4;

	const static int DEFAULT_BUFF_SIZE = 256;

public:
	OutPacket();
	OutPacket(short opcode);
	~OutPacket();

	int GetPacketSize() const
	{
		return nPacketSize - 4;
	}

	unsigned char* GetPacket()
	{
		return aBuff + 4;
	}

	void CopyFromTransferedPacket(InPacket *oPacket);
	void Encode1(char value);
	void Encode2(short value);
	void Encode4(int value);
	void Encode8(long long int value);
	void EncodeBuffer(unsigned char *buff, int size);
	void EncodeStr(const std::string &str);
	void EncodeHexString(const std::string& str);
	void EncodeTime(int64_t timeValue);
	void Release();

	void Print()
	{
		printf("OutPacket«Ê¥]¤º®e¡G");
		for (unsigned int i = 0; i < nPacketSize; ++i)
			printf("0x%02X ", (int)aBuff[i]);
		printf("\n");
	}
};

