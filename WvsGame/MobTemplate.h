#pragma once

#include <map>
#include "..\WvsLib\Wz\WzResMan.hpp"

class GW_MobReward;

class MobTemplate
{
	static std::map<int, MobTemplate*>* m_MobTemplates;
	static WZ::Node* m_MobWzProperty;


public:
	GW_MobReward* m_pReward;
	MobTemplate();
	~MobTemplate();

	//cloneNewOne為true，代表不使用共用屬性 (用於複寫特定怪物屬性)
	static MobTemplate* GetMobTemplate(int dwTemplateID, bool cloneNewOne = false);
	static void RegisterMob(int dwTemplateID);
	
	/*
	直接作為public attribute
	*/
	long long int m_lnMaxHP, m_lnMaxMP;

	int m_nLevel, m_nSpeed, m_nPAD, m_nPDD, m_nPDRate, m_nMAD, m_nMDD, m_nMDRate, m_nACC, m_nEVA, m_nPushed, m_nEXP, m_nCategory, m_nSummonType, m_nMoveAbility;

	bool m_bIsBodyAttack, m_bIsChangeableMob, m_bIsDamagedByMob, m_bIsSmartPhase, bIsTargetFromSrv = true;

	double m_dFs;

	std::string m_strElemAttr, m_strMobType;

	/*void SetMaxHP(long long int maxHp);
	void SetMaxMP(long long int maxMp);

	void SetLevel(int level);
	void SetSpeed(int speed);
	void SetPAD(int PAD);
	void SetPDD(int PDD);
	void SetPDRate(int rate);
	void SetMAD(int MAD);
	void SetMDD(int MDD);
	void SetMDRate(int rate);
	void SetACC(int ACC);
	void SetEVA(int EVA);
	void SetPushed(int pushed);
	void SetEXP(int EXP);
	void SetCategory(int category);

	void SetElementAttribute(const std::string& elemAttr);
	void SetMobType(const std::string& mobType);

	long long int GetMaxHP() const;
	long long int GetMaxMP() const;
		
	int GetLevel() const;
	int GetSpeed() const;
	int GetPAD() const;
	int GetPDD() const;
	int GetPDRate() const;
	int GetMAD() const;
	int GetMDD() const;
	int GetMDRate() const;
	int GetACC() const;
	int GetEVA() const;
	int GetPushed() const;
	int GetEXP() const;
	int GetCategory() const;

	const std::string& GetElementAttribute() const;
	const std::string& GetMobType() const;*/

	const GW_MobReward* GetMobReward() const;
};

