#pragma once
#include <vector>

struct GW_SkillRecord;
class User;

class QWUSkillRecord
{
	QWUSkillRecord();
	~QWUSkillRecord();
public:

	static bool SkillUp(User *pUser, int nSkillID, int nAmount, bool bDecSP, bool bCheckMasterLevel, std::vector<GW_SkillRecord*> &aChange);
	static void SendCharacterSkillRecord(User *pUser, std::vector<GW_SkillRecord*> &aChange);
};

