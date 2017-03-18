#pragma once
#include <string>

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

	void Encode1(char value);
	void Encode2(short value);
	void Encode4(int value);
	void Encode8(long long int value);
	void EncodeBuffer(unsigned char *buff, int size);
	void EncodeStr(const std::string &str);

};

