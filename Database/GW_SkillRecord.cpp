#include "GW_SkillRecord.h"
#include "WvsUnified.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

void GW_SkillRecord::Encode(OutPacket * oPacket)
{
	oPacket->Encode4(nSkillID);
	oPacket->Encode4(nSLV);
	oPacket->Encode8(tExpired);
	//WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "SkillID = %d, Need Master Level = %d Master Level = %d\n", nSkillID, (int)WvsGameConstants::IsSkillNeedMasterLevel(nSkillID), nMasterLevel);
	if (WvsGameConstants::IsSkillNeedMasterLevel(nSkillID))
		oPacket->Encode4(nMasterLevel);
	if (nSkillID == 0x262A822 || nSkillID == 80000004)
		oPacket->Encode4(0); //unk
}

void GW_SkillRecord::Decode(InPacket * iPacket)
{
	nSkillID = iPacket->Decode4();	
	nSLV = iPacket->Decode4();
	tExpired = iPacket->Decode8();
	if (WvsGameConstants::IsSkillNeedMasterLevel(nSkillID))
		nMasterLevel = iPacket->Decode4();
	if (nSkillID == 0x262A822 || nSkillID == 80000004)
		iPacket->Decode4();
}

void GW_SkillRecord::Load(void * pRecordSet)
{
	Poco::Data::RecordSet &recordSet = *((Poco::Data::RecordSet*)pRecordSet);
	nCharacterID = recordSet["CharacterID"];
	nSkillID = recordSet["SkillID"];
	nSLV = recordSet["SLV"];
	nMasterLevel = recordSet["MasterLevel"];
	tExpired = recordSet["Expired"];
}

void GW_SkillRecord::Save()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	//queryStatement << "DELETE FROM SkillRecord Where SkillID = " << nSkillID << " and CharacterID = " << nCharacterID;
	//queryStatement.execute();
	
	//queryStatement.reset(GET_DB_SESSION);
	queryStatement << "INSERT INTO SkillRecord VALUES("
		<< nCharacterID << ", "
		<< nSkillID << ", "
		<< nSLV << ", "
		<< nMasterLevel << ", "
		<< tExpired << ") ON DUPLICATE KEY UPDATE "
		<< "SLV = " << nSLV << ", "
		<< "MasterLevel = " << nMasterLevel << ", "
		<< "Expired = " << tExpired
		;
	queryStatement.execute();
}

