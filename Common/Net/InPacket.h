#pragma once
#include <string>

class InPacket
{
private:
	unsigned char* aBuff;
	unsigned short nPacketSize, nReadPos;

public:
	InPacket(unsigned char* buff, unsigned short size);
	~InPacket();

	char Decode1();
	short Decode2();
	int Decode4();
	long long int Decode8();
	std::string DecodeStr();
	void DecodeBuffer(unsigned char* dst, int size);

	void Print();
};