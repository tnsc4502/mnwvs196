#pragma once
#include <string>
#include <vector>

class InPacket;
class OutPacket;

struct GW_QuestRecord
{
	std::vector<int> aMobRecord;

	int nQuestID = 0,
		nState = 0,
		nCharacterID = 0;

	long long int tTime = 0;

	std::string sStringRecord;

#ifdef DBLIB
	void Load(void* pRecordSet);
	void Save();
#endif

	void Encode(OutPacket *oPacket);
	void Decode(InPacket *iPacket, int nState);
};

