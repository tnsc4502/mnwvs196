#include "QWUSkillRecord.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_SkillRecord.h"
#include "User.h"
#include "SkillInfo.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

QWUSkillRecord::QWUSkillRecord()
{
}


QWUSkillRecord::~QWUSkillRecord()
{
}

bool QWUSkillRecord::SkillUp(User * pUser, int nSkillID, int nAmount, bool bDecSP, bool bCheckMasterLevel, std::vector<GW_SkillRecord*>& aChange)
{
	int nJob = pUser->GetCharacterData()->mStat->nJob;
	int nSkillJob = WvsGameConstants::GetSkillRootFromSkill(nSkillID);

	if ((nJob >= nSkillJob) &&
		((nJob < 10000 && (nJob / 100 == nSkillJob / 100)) ||
		(nJob / 1000 == nSkillJob / 1000)))
	{
		int nSkillRootLevel = WvsGameConstants::GetJobLevel(nSkillJob);
		if (nSkillRootLevel >= 0 &&
			nSkillRootLevel < GW_CharacterStat::EXTEND_SP_SIZE &&
			(!bDecSP || pUser->GetCharacterData()->mStat->aSP[nSkillRootLevel] >= nAmount))
		{
			auto pSkillRecord = pUser->GetCharacterData()->GetSkill(nSkillID);
			if (pSkillRecord == nullptr)
			{
				pSkillRecord = SkillInfo::GetInstance()->GetSkillRecord(
					nSkillID,
					0,
					0
				);
				pUser->GetCharacterData()->ObtainSkillRecord(pSkillRecord);
			}
			if (pSkillRecord != nullptr 
				&& (!bCheckMasterLevel 
					|| (!WvsGameConstants::IsSkillNeedMasterLevel(pSkillRecord->nSkillID)
						|| (pSkillRecord->nSLV + nAmount <= pSkillRecord->nMasterLevel))
					)
				)
			{
				pSkillRecord->nSLV += nAmount;
				if(bDecSP)
					pUser->GetCharacterData()->mStat->aSP[nSkillRootLevel] -= nAmount;
				aChange.push_back(pSkillRecord);
				return true;
			}
		}
	}
	return false;
}

void QWUSkillRecord::SendCharacterSkillRecord(User * pUser, std::vector<GW_SkillRecord*>& aChange)
{
	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "SendCharacterSkillRecord Called, size = %d\n", (int)aChange.size());
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnChangeSkillRecordResult);
	oPacket.Encode1(1);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode2((short)aChange.size());
	for (auto pSkillRecord : aChange)
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "SendCharacterSkillRecord Skill ID = %d SLV = %d\n", pSkillRecord->nSkillID, pSkillRecord->nSLV);
		oPacket.Encode4(pSkillRecord->nSkillID);
		oPacket.Encode4(pSkillRecord->nSLV);
		oPacket.Encode4(pSkillRecord->nMasterLevel);
		oPacket.Encode8(pSkillRecord->tExpired);
	}
	oPacket.Encode1(0);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	pUser->SendPacket(&oPacket);
}

