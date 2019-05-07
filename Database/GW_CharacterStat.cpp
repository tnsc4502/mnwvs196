#include "GW_CharacterStat.h"
#include "WvsUnified.h"
#include "..\WvsLib\String\StringUtility.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

GW_CharacterStat::GW_CharacterStat()
{
	for (auto& sp : aSP)
		sp = 0;
}

GW_CharacterStat::~GW_CharacterStat()
{
}

void GW_CharacterStat::EncodeExtendSP(OutPacket * oPacket)
{
	if (WvsGameConstants::IsExtendSPJob(nJob))
	{
		int nCount = 0;
		for (auto sp : aSP)
			if (sp != 0)
				++nCount;
		oPacket->Encode1(nCount);
		for (int i = 0; i < EXTEND_SP_SIZE; ++i)
		{
			if (aSP[i] <= 0)
				continue;
			oPacket->Encode1(i);
			oPacket->Encode4(aSP[i]);
		}
	}
	else
		oPacket->Encode2(aSP[0]);
}

void GW_CharacterStat::DecodeExtendSP(InPacket * iPacket)
{
	for (auto& sp : aSP)
		sp = 0;
	if (WvsGameConstants::IsExtendSPJob(nJob))
	{
		int nCount = iPacket->Decode1();
		int nLVL = 0;
		for (int i = 0; i < nCount; ++i)
		{
			nLVL = iPacket->Decode1();
			aSP[nLVL] = iPacket->Decode4();
		}
	}
	else
		aSP[0] = iPacket->Decode2();
}

void GW_CharacterStat::Load(int nCharacterID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM CharacterStat Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	nHP = recordSet["HP"];
	nMP = recordSet["MP"];
	nMaxHP = recordSet["MaxHP"];
	nMaxMP = recordSet["MaxMP"]; 
	nGender = recordSet["Gender"];
	nFame = recordSet["Fame"];
	nJob = recordSet["Job"];
	nSubJob = recordSet["SubJob"];
	nStr = recordSet["Str"];
	nDex = recordSet["Dex"];
	nInt = recordSet["Int_"];
	nLuk = recordSet["Luk"];
	nSkin = recordSet["Skin"];
	nFace = recordSet["Face"];
	nHair = recordSet["Hair"];
	nFaceMark = recordSet["FaceMark"];

	auto strSP = (std::string)recordSet["SP"].toString();
	std::vector<std::string> split;
	StringUtility::Split(strSP, split, ",");
	for (int i = 0; i < EXTEND_SP_SIZE; ++i)
		aSP[i] = atoi(split[i].c_str());

	nAP = recordSet["AP"];
	nExp = recordSet["Exp"];
	nPOP = recordSet["POP"];
	nCharismaEXP = recordSet["CharismaEXP"];
	nInsightEXP = recordSet["InsightEXP"];
	nWillEXP = recordSet["WillEXP"];
	nSenseEXP = recordSet["SenseEXP"];
	nCharmEXP = recordSet["CharmEXP"];
}

void GW_CharacterStat::Save(int nCharacterID, bool isNewCharacter)
{
	if (isNewCharacter)
	{
		Poco::Data::Statement newRecordStatement(GET_DB_SESSION);
		newRecordStatement << "INSERT INTO CharacterStat(CharacterID) VALUES(" << nCharacterID << ")";
		newRecordStatement.execute();
	}
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	std::string strSP = "";
	for (int i = 0; i < EXTEND_SP_SIZE; ++i)
		strSP += (std::to_string(aSP[i]) + (i == EXTEND_SP_SIZE - 1 ? "" : ","));

	queryStatement << "UPDATE CharacterStat Set "
		<< "Exp = '" << nExp << "', "
		<< "HP = " << nHP << ", "
		<< "MP = " << nMP << ", "
		<< "MaxHP = " << nMaxHP << ", "
		<< "MaxMP = " << nMaxMP << ", "
		<< "Gender = '" << nGender << "', "
		<< "Fame = '" << nFame << "', "
		<< "Job = '" << nJob << "', "
		<< "SubJob = '" << nSubJob << "', "
		<< "Str = " << nStr << ", "
		<< "Dex = " << nDex << ", "
		<< "Int_ = " << nInt << ", "
		<< "Luk = " << nLuk << ", "
		<< "SP = '" << strSP << "', "
		<< "POP = " << nPOP << ", "
		<< "CharismaEXP = " << nCharismaEXP << ", "
		<< "InsightEXP = " << nInsightEXP << ", "
		<< "WillEXP = " << nWillEXP << ", "
		<< "SenseEXP = " << nSenseEXP << ", "
		<< "CharmEXP = " << nCharmEXP << ", "
		<< "Hair = " << nHair << ","
		<< "Face = " << nFace << ","
		<< "Skin = " << nSkin << ","
		<< "FaceMark = " << nFaceMark << ", "
		<< "AP = '" << nAP << "' WHERE CharacterID = " << nCharacterID;
	queryStatement.execute();
}