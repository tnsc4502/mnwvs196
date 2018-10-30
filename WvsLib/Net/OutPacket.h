#pragma once
#include <string>
#include <atomic>

class InPacket;

class OutPacket
{
public:
	class SharedPacket {
	private:
		friend class OutPacket;

		unsigned char* aBuff = nullptr;
		unsigned int nBuffSize = 0, nPacketSize = 4 + 8;
		std::atomic<int> nRefCount;

		const static int DEFAULT_BUFF_SIZE = 256;

	public:
		SharedPacket();
		~SharedPacket();

		void IncRefCount();
		void DecRefCount();
	};

private:
	SharedPacket* m_pSharedPacket;

public:

	OutPacket();
	//OutPacket(short opcode);
	~OutPacket();

	int GetPacketSize() const
	{
		return m_pSharedPacket->nPacketSize - (4 + 8);
	}

	unsigned char* GetPacket()
	{
		return m_pSharedPacket->aBuff + (4 + 8); // +8 for this pointer
	}

	void CopyFromTransferedPacket(InPacket *oPacket);
	void Encode1(char value);
	void Encode2(short value);
	void Encode4(int value);
	void Encode8(long long int value);
	void EncodeBuffer(unsigned char *buff, int size);
	void EncodeStr(const std::string &str);
	void EncodeHexString(const std::string& str);
	void Release();

	void IncRefCount();
	void DecRefCount();

	void Print();

	SharedPacket* GetSharedPacket();
};

