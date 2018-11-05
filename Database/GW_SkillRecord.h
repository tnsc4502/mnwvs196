#pragma once
class OutPacket;
class InPacket;

struct GW_SkillRecord
{
	int nSLV, nSkillID, nMasterLevel, nCharacterID;

	long long int tExpired;

	void Encode(OutPacket* oPacket);
	void Decode(InPacket* iPacket);

#ifdef DBLIB
	void Load(void *pRecordSet);
	void Save();
#endif
};

