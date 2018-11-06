#include "GW_FuncKeyMapped.h"
#include "WvsUnified.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

int aDefaultKeyMapped[][3] = 
{
	{30, 5, 50 },
	{44, 5, 50},
	{2, 4, 10},
	{3, 4, 12},
	{4, 4, 13},
	{5, 4, 18},
	{6, 4, 23},
	{16, 4, 8},
	{17, 4, 5},
	{18, 4, 0},
	{19, 4, 4},
	{20, 4, 27},
	{21, 4, 30},
	{22, 4, 39},
	{23, 4, 1},
	{24, 4, 41},
	{25, 4, 19},
	{26, 4, 14},
	{27, 4, 15},
	{29, 5, 52},
	{31, 4, 2},
	{34, 4, 17},
	{35, 4, 11},
	{37, 4, 3},
	{38, 4, 20},
	{39, 4, 26},
	{40, 4, 16},
	{41, 4, 22},
	{43, 4, 9},
	{45, 5, 51},
	{46, 4, 6},
	{47, 4, 31},
	{48, 4, 29},
	{50, 4, 7},
	{52, 4, 46},
	{56, 5, 53},
	{57, 5, 54},
	{59, 6, 100},
	{60, 6, 101},
	{61, 6, 102},
	{63, 6, 103},
	{64, 6, 104},
	{65, 6, 105},
	{66, 6, 106}
};

GW_FuncKeyMapped::GW_FuncKeyMapped(int nCharacterID)
{
	this->nCharacterID = nCharacterID;
}

GW_FuncKeyMapped::~GW_FuncKeyMapped()
{
}

void GW_FuncKeyMapped::Load()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM FuncKeyMapped Where Type > 0 AND CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	int nKey = 0;
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		nKey = (int)recordSet["Key"];
		auto& ref = m_mKeyMapped[nKey];
		ref.nType = (int)recordSet["Type"];
		ref.nValue = (int)recordSet["Value"];
	}
}

void GW_FuncKeyMapped::Save(bool bNewCharacter)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	if (bNewCharacter)
	{
		for (auto& aKeyInfo : aDefaultKeyMapped)
		{
			auto &ref = m_mKeyMapped[aKeyInfo[0]];
			ref.nKey = aKeyInfo[0];
			ref.nType = aKeyInfo[1];
			ref.nValue = aKeyInfo[2];
		}
		/*for (int i = 0; i < TOTAL_KEY_NUM; ++i)
		{
			if (i == 0)
				queryStatement << "INSERT INTO FuncKeyMapped VALUES(";
			else
				queryStatement << ", (";
			queryStatement
				<< nCharacterID << ", "
				<< i << ", "
				<< 0 << ", "
				<< 0 << ") ";
		}*/
		//return;
	}
	if (m_mKeyMapped.size() == 0)
		return;
	queryStatement << "INSERT INTO FuncKeyMapped(`CharacterID`, `Key`, `Type`, `Value`) VALUES";
	auto lastRecord = *m_mKeyMapped.rbegin();
	for (auto& keyMapped : m_mKeyMapped)
	{
		queryStatement << "("
			<< nCharacterID << ", "
			<< keyMapped.second.nKey << ", "
			<< keyMapped.second.nType << ", "
			<< keyMapped.second.nValue << ") ";
		if (keyMapped.first != lastRecord.first)
			queryStatement << ", ";
	}
	queryStatement << " ON DUPLICATE KEY UPDATE CharacterID = VALUES (CharacterID), "
		<< "`Key`=VALUES(`Key`),"
		<< "`Type`=VALUES(`Type`),"
		<< "`Value`=VALUES(`Value`)";

	queryStatement.execute();
}

void GW_FuncKeyMapped::Encode(OutPacket * oPacket, bool bModifiedOnly)
{
	oPacket->Encode1(m_mKeyMapped.size() == 0);
	auto end = m_mKeyMapped.end();
	for (int i = 0; i < TOTAL_KEY_NUM; ++i)
	{
		auto findIter = m_mKeyMapped.find(i);
		if (findIter != end && (!bModifiedOnly || (findIter->second.bModified)))
		{
			oPacket->Encode1(findIter->second.nType);
			oPacket->Encode4(findIter->second.nValue);
		}
		else
		{
			oPacket->Encode1(0);
			oPacket->Encode4(0);
		}
	}
}

void GW_FuncKeyMapped::Decode(InPacket * iPacket, bool bDecodeEmpty)
{
	bool bEmpty = iPacket->Decode1() == 1;
	int nKey = 0, nType = 0, nValue = 0;
	if (!bEmpty)
	{
		for (int i = 0; i < TOTAL_KEY_NUM; ++i)
		{
			nKey = i;
			nType = iPacket->Decode1();
			nValue = iPacket->Decode4();
			if (bDecodeEmpty || (nType || nValue))
			{
				auto &ref = m_mKeyMapped[i];
				ref.nKey = nKey;
				ref.nType = nType;
				ref.nValue = nValue;
			}
		}
	}
}
