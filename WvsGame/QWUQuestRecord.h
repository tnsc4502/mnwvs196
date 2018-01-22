#pragma once
#include <string>
class User;

class QWUQuestRecord
{
	QWUQuestRecord() = delete;
	~QWUQuestRecord() = delete;
public:
	static int GetState(User *pUser, int nKey);
	static void Remove(User *pUser, int nKey, bool bComplete);
	static std::string Get(User *pUser, int nKey);
	static void Set(User *pUser, int nKey, const std::string & sInfo);
	static void SetComplete(User *pUser, int nKey);
};

