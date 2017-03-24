#include "GW_CharacterStat.h"
#include "WvsUnified.h"

GW_CharacterStat::GW_CharacterStat()
{
}


GW_CharacterStat::~GW_CharacterStat()
{
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
	nJob = recordSet["Job"];
	nSubJob = recordSet["SubJob"];
	nStr = recordSet["Str"];
	nDex = recordSet["Dex"];
	nInt = recordSet["Int_"];
	nLuk = recordSet["Luk"];
	strSP = (std::string)recordSet["SP"].toString();
	nAP = recordSet["AP"];
	nExp = recordSet["Exp"];
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
	queryStatement << "UPDATE CharacterStat Set "
		<< "Exp = '" << nExp << "', "
		<< "HP = '" << nHP << "', "
		<< "MP = '" << nMP << "', "
		<< "MaxHP = '" << nMaxHP << "', "
		<< "MaxMP = '" << nMaxMP << "', "
		<< "Job = '" << nJob << "', "
		<< "SubJob = '" << nSubJob << "', "
		<< "Str = '" << nStr << "', "
		<< "Dex = '" << nDex << "', "
		<< "Int_ = '" << nInt << "', "
		<< "Luk = '" << nLuk << "', "
		<< "SP = '" << strSP << "', "
		<< "AP = '" << nAP << "' WHERE CharacterID = " << nCharacterID;
	queryStatement.execute();
}