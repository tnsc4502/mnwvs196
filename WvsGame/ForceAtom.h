#pragma once

#include <vector>
#include "FieldRect.h"
#include "FieldPoint.h"

class OutPacket;
class User;
class Field;

struct ForceAtom
{
	enum ForceAtomType
	{
		e_DemonFury = 0x00, //惡魔之力
		e_PhantomCard = 0x01, //幻影卡牌
		e_TempestBlade = 0x02, //意志之劍
		e_NetherShield = 0x03, //盾牌追擊
		e_AegisSystem = 0x05, //神盾系統
		e_PinpointSalvo = 0x06, //追縱火箭
		e_TriflingWind = 0x07, //風妖精之箭
		e_StormBringer = 0x08, //風暴使者

		e_QuiverCartridge = 0x0A, //魔幻箭筒
		e_AssassinsMark = 0x0B, //刺客刻印
		e_MesoExplosion = 0x0C, //楓幣炸彈
		e_FoxSpirits = 0x0D, //小狐仙

		e_ShadowBat = 0x0F, //暗影蝙蝠
		e_OrbitalFlame = 0x10, //元素火焰

		e_HomingBeacon = 0x14, //35101002, 追蹤飛彈
		e_LightningFusion = 0x1B, //神雷合一
		e_FireUmpire = 0x1C, //400021001, 持續制裁者
		e_EnergeticExplosion = 0x1D, //400051011, 能量爆炸
	};

	struct ForceAtomInfo
	{
		int dwKey = 0, 
			nInc = 0, 
			nFirstImpact = 0, 
			nSecondImpact = 0,
			nAngle = 0,
			nStartDelay = 0,
			dwCreateTime = 0,
			nMaxHitCount = 0,
			nEffectIdx = 0;

		FieldPoint ptStart;
	};

	FieldRect rcStart;
	bool m_bByMob = false, m_bToMob = false;

	int m_dwUserOwner = 0,
		m_dwTargetID = 0,
		m_nSkillID = 0,
		m_nForceAtomType = 0,
		m_dwFirstMobID = 0,
		m_nBulletItemID = 0,
		m_nArriveDir = 0,
		m_nArriveRange = 0;

	FieldPoint m_ptForcedTarget;
	std::vector<int> m_adwTargetMob;
	std::vector<ForceAtomInfo> m_aForceAtomInfo;

	void AddForceAtomInfo(ForceAtomType nType, int nInc, Field* pField, const FieldPoint& pt);
	void CreateForceAtom(int dwUserOwner, int nSkillID, bool bByMob, bool bToMob, int dwTargetID, ForceAtomType nType, int nInc, int nCount, Field* pField, const FieldPoint& pt);
	void OnForceAtomCreated(Field *pField);
};

