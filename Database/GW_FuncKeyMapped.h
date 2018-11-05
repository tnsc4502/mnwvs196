#pragma once
#include <map>

class InPacket;
class OutPacket;

struct GW_FuncKeyMapped
{
	static const int TOTAL_KEY_NUM = 89;

	struct FuckKeyMapped
	{
		bool bModified = false;
		int nKey = 0,
			nType = 0,
			nValue = 0;
	};

	std::map<int, FuckKeyMapped> m_mKeyMapped;
	int nCharacterID = 0;

	GW_FuncKeyMapped(int nCharacterID);
	~GW_FuncKeyMapped();

#ifdef DBLIB
	void Load();
	void Save(bool bNewCharacter = false);
#endif

	void Encode(OutPacket *oPacket, bool bModifiedOnly = false);
	void Decode(InPacket *iPacket, bool bDecodeEmpty = true);
};

