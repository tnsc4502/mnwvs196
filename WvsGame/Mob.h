#pragma once
#include <map>
#include "FieldObj.h"

class MobTemplate;
class Controller;
class User;

class Mob : public FieldObj
{
private:
	MobTemplate* m_pMobTemplate;

	std::map<int, long long int> m_mAttackRecord;
	Controller* m_pController;

	long long int m_liHP, m_liMP;

public:
	Mob();
	~Mob();

	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void EncodeInitData(OutPacket *oPacket, bool bIsControl = false);
	void SendChangeControllerPacket(User* pUser, int nLevel);
	void SendReleaseControllPacket(User* pUser, int dwMobID);
	void SetMobTemplate(MobTemplate *pTemplate);
	const MobTemplate* GetMobTemplate() const;

	void SetController(Controller* pController);
	Controller* GetController();

	void SetMovePosition(int x, int y, char bMoveAction, short nSN);

	//解析怪物移動時，Lucid有些怪物移動封包多兩個bytes
	static bool IsLucidSpecialMob(int dwTemplateID);
	void OnMobHit(User* pUser, long long int nDamage, int nAttackType);
	void OnMobDead(int nHitX, int nHitY, int nMesoUp, int nMesoUpByItem);
	void DistributeExp(int& refOwnType, int& refOwnParyID, int& refLastDamageCharacterID);
	void GiveReward(unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, int nX, int nY, int tDelay, int nMesoUp, int nMesoUpByItem);

	void SetHP(long long int liHP);
	void SetMP(long long int liMP);
	long long int GetHP() const;
	long long int GetMP() const;

	std::pair<int, int> GetDropPos();
};

