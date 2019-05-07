#pragma once
#include <string>
#include <atomic>

class InPacket;

class OutPacket
{
public:
	const static int
		DEFAULT_BUFF_SIZE = 256,
		THIS_PTR_OFFSET = 8, //8 bytes are reserved for 64-bit addressing
		HEADER_OFFSET = 4, //4 bytes are for packet header
		INITIAL_WRITE_INDEX = THIS_PTR_OFFSET + HEADER_OFFSET;

	class SharedPacket {
	private:
		friend class OutPacket;

		unsigned char* aBuff = nullptr;
		unsigned int nBuffSize = 0, nPacketSize = INITIAL_WRITE_INDEX;
		bool m_bBroadcasting = false;

		std::atomic<int> nRefCount;

	public:
		SharedPacket();
		~SharedPacket();

		void ToggleBroadcasting();
		bool IsBroadcasting() const;

		void IncRefCount();
		void DecRefCount();
	};

	void ExtendSize(int nExtendRate);

private:
	SharedPacket* m_pSharedPacket;

public:

	OutPacket();
	//OutPacket(short opcode);
	~OutPacket();

	int GetPacketSize() const
	{
		return m_pSharedPacket->nPacketSize - (INITIAL_WRITE_INDEX);
	}

	unsigned char* GetPacket()
	{
		return m_pSharedPacket->aBuff + (INITIAL_WRITE_INDEX); // +8 for this pointer
	}

	void CopyFromTransferedPacket(InPacket *oPacket);
	void Encode1(char value);
	void Encode2(short value);
	void Encode4(int value);
	void Encode8(long long int value);
	void EncodeBuffer(unsigned char *buff, int nSize, int nZero = 0);
	void EncodeStr(const std::string &str);
	void EncodeHexString(const std::string& str);
	void Release();
	void Reset();

	void IncRefCount();
	void DecRefCount();

	void Print();

	SharedPacket* GetSharedPacket();
};

