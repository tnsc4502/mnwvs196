#include "MobTemplate.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\Database\GW_MobReward.h"

std::map<int, MobTemplate*>* MobTemplate::m_MobTemplates = new std::map<int, MobTemplate*>();

WZ::Node* MobTemplate::m_MobWzProperty = &(stWzResMan->GetWz(Wz::Mob));

MobTemplate::MobTemplate()
{
}

MobTemplate::~MobTemplate()
{
}

//cloneNewOne為true，代表不使用共用屬性 (用於複寫特定怪物屬性)
MobTemplate* MobTemplate::GetMobTemplate(int dwTemplateID, bool cloneNewOne)
{
	auto iter = m_MobTemplates->find(dwTemplateID);
	if (!cloneNewOne &&  iter != m_MobTemplates->end())
		return (*m_MobTemplates)[dwTemplateID];

	if (iter == m_MobTemplates->end()) {
		RegisterMob(dwTemplateID); 
		iter = m_MobTemplates->find(dwTemplateID);
	}
	if (!cloneNewOne) 
	{
		MobTemplate* clone = AllocObj(MobTemplate);
		*clone = *(iter->second);
		return clone;
	}
	return iter->second;
}

void MobTemplate::RegisterMob(int dwTemplateID)
{
	m_MobWzProperty = &(stWzResMan->GetWz(Wz::Mob));
	auto newTemplate = AllocObj(MobTemplate);
	std::string templateID = std::to_string(dwTemplateID);
	while (templateID.length() < 7)
		templateID = "0" + templateID;
	auto& info = (*m_MobWzProperty)[templateID]["info"];
	newTemplate->m_nLevel = info["level"];
	newTemplate->m_bIsBodyAttack = ((int)info["bodyAttack"] == 1);
	newTemplate->m_bIsChangeableMob = ((int)info["changeableMob"] == 1);
	newTemplate->m_bIsDamagedByMob = ((int)info["damagedByMob"] == 1);
	newTemplate->m_bIsSmartPhase = ((int)info["smartPhase"] == 1);
	newTemplate->m_lnMaxHP = (int)info["maxHP"];
	newTemplate->m_lnMaxMP = (int)info["maxMP"];
	newTemplate->m_nSpeed = info["speed"];
	newTemplate->m_nPAD = info["PADamage"];
	newTemplate->m_nPDD = info["PDDamage"];
	newTemplate->m_nPDRate = info["PDRate"];
	newTemplate->m_nMAD = info["MADamage"];
	newTemplate->m_nMDD = info["MDDamage"];
	newTemplate->m_nMDRate = info["MDRate"];
	newTemplate->m_nACC = info["acc"];
	newTemplate->m_nEVA = info["eva"];
	newTemplate->m_nPushed = info["pushed"];
	newTemplate->m_dFs = info["fs"];
	newTemplate->m_nSummonType = info["summonType"];
	newTemplate->m_nEXP = info["exp"];
	newTemplate->m_nCategory = info["category"];
	newTemplate->m_strElemAttr = info["elemAttr"];
	newTemplate->m_strMobType = info["mobType"];

	bool bFly = (info["fly"] == info.end());
	bool bMove = (info["move"] == info.end());
	bool bJump = (info["jump"] == info.end());
	if (bFly)
		newTemplate->m_nMoveAbility = 3;
	else if (bJump)
		newTemplate->m_nMoveAbility = 2;
	else
		newTemplate->m_nMoveAbility = (bMove != false) ? 1 : 0;

	newTemplate->m_pReward = GW_MobReward::GetInstance()->GetMobReward(dwTemplateID);
	(*m_MobTemplates)[dwTemplateID] = newTemplate;
}

const GW_MobReward * MobTemplate::GetMobReward() const
{
	return m_pReward;
}
