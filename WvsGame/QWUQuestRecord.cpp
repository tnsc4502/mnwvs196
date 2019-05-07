#include "QWUQuestRecord.h"
#include "User.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_QuestRecord.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include <mutex>


int QWUQuestRecord::GetState(User * pUser, int nKey)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacter = pUser->GetCharacterData();
	auto findIter = pCharacter->mQuestRecord.find(nKey);
	if (findIter != pCharacter->mQuestRecord.end())
		return findIter->second->nState;

	findIter = pCharacter->mQuestComplete.find(nKey);
	if (findIter != pCharacter->mQuestComplete.end())
		return findIter->second->nState;

	return 0;
}

void QWUQuestRecord::Remove(User * pUser, int nKey, bool bComplete)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacter = pUser->GetCharacterData();
	if (bComplete)
		pCharacter->mQuestComplete.erase(nKey);
	else
		pCharacter->RemoveQuest(nKey);
	pUser->SendQuestRecordMessage(nKey, 0, "");
}

std::string QWUQuestRecord::Get(User * pUser, int nKey)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacter = pUser->GetCharacterData();
	auto findIter = pCharacter->mQuestRecord.find(nKey);
	if (findIter != pCharacter->mQuestRecord.end())
		return findIter->second->sStringRecord;
	return "";
}

void QWUQuestRecord::Set(User * pUser, int nKey, const std::string & sInfo)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacter = pUser->GetCharacterData();
	pCharacter->SetQuest(nKey, sInfo);
	pUser->SendQuestRecordMessage(nKey, 1, sInfo);
}

void QWUQuestRecord::SetComplete(User * pUser, int nKey)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacter = pUser->GetCharacterData();
	pCharacter->RemoveQuest(nKey);
	GW_QuestRecord *pComplete = AllocObj(GW_QuestRecord);
	pComplete->nQuestID = nKey;
	pComplete->nState = 2;
	pComplete->tTime = GameDateTime::GetTime();
	pCharacter->mQuestComplete.insert({ nKey , pComplete });
	pUser->SendQuestRecordMessage(nKey, 2, "");
}
