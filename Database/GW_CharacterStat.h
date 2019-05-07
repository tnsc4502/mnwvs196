#pragma once
#include <string>

class OutPacket;
class InPacket;

struct GW_CharacterStat
{
	static const int EXTEND_SP_SIZE = 10;

	unsigned char nGender = 0;

	int nHP = 0,
		nMP = 0,
		nMaxHP = 0,
		nMaxMP = 0,
		nFame = 0,
		nJob = 0,
		nSubJob = 0,
		nStr = 0,
		nDex = 0,
		nInt = 0,
		nLuk = 0,
		nAP = 0,
		nPOP = 0,
		nCharismaEXP = 0,
		nInsightEXP = 0,
		nWillEXP = 0,
		nSenseEXP = 0,
		nCharmEXP = 0,
		aSP[EXTEND_SP_SIZE],
		nFace = 0,
		nHair = 0,
		nSkin = 0,
		nFaceMark = 0;

	long long int nExp = 0;

public:
	GW_CharacterStat();
	~GW_CharacterStat();


	void EncodeExtendSP(OutPacket *oPacket);
	void DecodeExtendSP(InPacket *iPacket);

#ifdef DBLIB
	void Load(int nCharacterID);
	void Save(int nCharacterID, bool isNewCharacter);
#endif

};

