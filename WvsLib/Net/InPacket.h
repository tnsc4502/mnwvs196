#pragma once
#include <string>

class InPacket
{
private:
	unsigned char* aBuff;
	unsigned short nPacketSize, nReadPos;

	bool bIsProcessed = false;

public:
	InPacket(unsigned char* buff, unsigned short size);
	~InPacket();

	char Decode1();
	short Decode2();
	int Decode4();
	long long int Decode8();
	std::string DecodeStr();
	void DecodeBuffer(unsigned char* dst, int size);

	unsigned char* GetPacket() const;
	unsigned short GetPacketSize() const;
	unsigned short RemainedCount() const;
	unsigned short GetReadCount() const;

	void SetProcessed() { bIsProcessed = true; }
	bool IsProcessed() const { return bIsProcessed; }

	/*Used for transfer packet to new OnPacket entry.*/
	void RestorePacket();
	void Seek(int nPos);
	void Offset(int nOffset);

	void Print();
};