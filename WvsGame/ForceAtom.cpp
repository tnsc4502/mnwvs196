#include "ForceAtom.h"

#include "Field.h"
#include "LifePool.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"

void ForceAtom::AddForceAtomInfo(ForceAtomType nType, int nInc, Field * pField, const FieldPoint & pt)
{
	ForceAtomInfo info;
	info.dwKey = pField->GetLifePool()->GetForceAtomObjectID();
	info.nInc = nInc;

	switch (nType)
	{
		case e_DemonFury:
			info.nFirstImpact = (Rand32::GetInstance()->Random(0x23, 0x31));
			info.nSecondImpact = (Rand32::GetInstance()->Random(5, 6));
			info.nAngle = (Rand32::GetInstance()->Random(0x1E, 0x44));
			break;
		case e_PhantomCard:
			info.nFirstImpact = (Rand32::GetInstance()->Random(15, 32));
			info.nSecondImpact = (Rand32::GetInstance()->Random(7, 10));
			info.nAngle = (Rand32::GetInstance()->Random(9, 25));
			break;
		case e_TempestBlade:
			info.nFirstImpact = (Rand32::GetInstance()->Random(15, 20));
			info.nSecondImpact = (Rand32::GetInstance()->Random(20, 30));
			info.nStartDelay = (Rand32::GetInstance()->Random(1000, 1500));
			break;
		case e_NetherShield:
			info.nFirstImpact = (Rand32::GetInstance()->Random(0x10, 0x12));
			info.nSecondImpact = (Rand32::GetInstance()->Random(0x14, 0x1C));
			info.nAngle = (Rand32::GetInstance()->Random(0x22, 0x42));
			info.nStartDelay = 0x3C;
			break;
		case e_AegisSystem:
			info.nFirstImpact = 0x23;
			info.nSecondImpact = 0x05;
			info.nAngle = (Rand32::GetInstance()->Random(80, 90));
			info.nStartDelay = (Rand32::GetInstance()->Random(100, 500));
			break;
		case e_PinpointSalvo:
			info.nFirstImpact = (Rand32::GetInstance()->Random(15, 20));
			info.nSecondImpact = (Rand32::GetInstance()->Random(20, 30));
			info.nAngle = (Rand32::GetInstance()->Random(120, 150));
			info.nStartDelay = (Rand32::GetInstance()->Random(300, 900));
			break;
		case e_TriflingWind:
			info.nFirstImpact = (Rand32::GetInstance()->Random(40, 50));
			info.nSecondImpact = (Rand32::GetInstance()->Random(5, 9));
			info.nAngle = (Rand32::GetInstance()->Random(150, 180));
			info.nStartDelay = (Rand32::GetInstance()->Random(30, 40));
			break;
		case e_QuiverCartridge:
			info.nFirstImpact = (Rand32::GetInstance()->Random(10, 14));
			info.nSecondImpact = (Rand32::GetInstance()->Random(8, 9));
			info.nAngle = (Rand32::GetInstance()->Random(0xD3, 0x13A));
			info.nStartDelay = (Rand32::GetInstance()->Random(0x1E, 0x3B));
			break;
		case e_FoxSpirits:
			info.nFirstImpact = (Rand32::GetInstance()->Random(0x10, 0x12));
			info.nSecondImpact = 0x15;
			info.nAngle = 0x2E;
			info.nStartDelay = 0x276;
			break;
		case e_HomingBeacon:
			info.nFirstImpact = 50;
			info.nSecondImpact = (Rand32::GetInstance()->Random(11, 13));
			info.nAngle = (Rand32::GetInstance()->Random(0x1A, 0x27));
			info.nStartDelay = 500;
			break;
		case e_LightningFusion:
			info.nFirstImpact = (Rand32::GetInstance()->Random(10, 50));
			info.nSecondImpact = (Rand32::GetInstance()->Random(0, 15));
			info.nAngle = (Rand32::GetInstance()->Random(30, 50));
			info.nStartDelay = 0;
			break;
		case e_FireUmpire:
			info.nFirstImpact = (Rand32::GetInstance()->Random(36, 39));
			info.nSecondImpact = (Rand32::GetInstance()->Random(5, 6));
			info.nAngle = (Rand32::GetInstance()->Random(33, 64));
			info.nStartDelay = (Rand32::GetInstance()->Random(512, 544));
			break;
		case e_EnergeticExplosion:
			info.nFirstImpact = (Rand32::GetInstance()->Random(36, 39));
			info.nSecondImpact = (Rand32::GetInstance()->Random(5, 6));
			info.nAngle = (Rand32::GetInstance()->Random(33, 64));
			info.nStartDelay = (Rand32::GetInstance()->Random(512, 544));
			break;
		default:
			info.nFirstImpact = 0;
			info.nSecondImpact = 0;
			info.nAngle = 0;
			info.nStartDelay = 0;
			break;
	}

	if (pt.x != 0 || pt.y != 0)
	{
		info.ptStart.x = Rand32::GetInstance()->Random(-600 + pt.x, 600 + pt.x);
		info.ptStart.y = Rand32::GetInstance()->Random(-300 + pt.y, 50 + pt.y);
	}
	else
	{
		info.ptStart.x = 0;
		info.ptStart.y = (
				nType == e_PhantomCard ? Rand32::GetInstance()->Random(0, 13) : 0
			);
	}

	info.dwCreateTime = GameDateTime::GetTime();
	info.nMaxHitCount = 10;
	info.nEffectIdx = 0;

	m_aForceAtomInfo.push_back(info);
}

void ForceAtom::CreateForceAtom(int dwUserOwner, int nSkillID, bool bByMob, bool bToMob, int dwTargetID, ForceAtomType nType, int nInc, int nCount, Field * pField, const FieldPoint & pt)
{
	for (int i = 0; i < nCount; ++i)
		AddForceAtomInfo(nType, nInc, pField, pt);
	m_bByMob = bByMob;
	m_bToMob = bToMob;
	m_nForceAtomType = nType;
	m_nSkillID = nSkillID;
	m_dwUserOwner = dwUserOwner;
	m_dwTargetID = dwTargetID;
}

void ForceAtom::OnForceAtomCreated(Field *pField)
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_OnCreateForceAtom);
	oPacket.Encode1(m_bByMob);
	if(m_bByMob)
		oPacket.Encode4(m_dwUserOwner);
	oPacket.Encode4(m_dwTargetID);
	oPacket.Encode4(m_nForceAtomType);
	if (!(m_nForceAtomType == 0 || m_nForceAtomType == 9 || m_nForceAtomType == 14 || m_nForceAtomType == 29))
	{
		oPacket.Encode1(m_bToMob);
		switch (m_nForceAtomType)
		{
		case e_TempestBlade:
		case e_NetherShield:
		case e_PinpointSalvo:
		case e_TriflingWind:
		case e_AssassinsMark:
		case e_MesoExplosion:
		case e_FoxSpirits:
		case 17:
		case 19:
		case e_HomingBeacon:
		case 23:
		case 24:
		case 25:
		case e_LightningFusion:
		case e_FireUmpire:
		{
			oPacket.Encode4((int)m_adwTargetMob.size());
			for (auto& value : m_adwTargetMob)
				oPacket.Encode4(value);
			break;
		}
		default:
			oPacket.Encode4(m_dwFirstMobID);
		}
		oPacket.Encode4(m_nSkillID);
	}

	for (const auto& info : m_aForceAtomInfo)
	{
		oPacket.Encode1(1);
		oPacket.Encode4(info.dwKey);
		oPacket.Encode4(info.nInc);
		oPacket.Encode4(info.nFirstImpact);
		oPacket.Encode4(info.nSecondImpact);
		oPacket.Encode4(info.nAngle);
		oPacket.Encode4(info.nStartDelay);
		oPacket.Encode4(info.ptStart.x);
		oPacket.Encode4(info.ptStart.y);
		oPacket.Encode4(info.dwCreateTime);
		oPacket.Encode4(info.nMaxHitCount);
		oPacket.Encode4(info.nEffectIdx);
	}
	oPacket.Encode1(0);

	if (m_nForceAtomType == e_AssassinsMark)
	{
		oPacket.Encode4(rcStart.left);
		oPacket.Encode4(rcStart.top);
		oPacket.Encode4(rcStart.right);
		oPacket.Encode4(rcStart.bottom);
		oPacket.Encode4(m_nBulletItemID);
	}
	if (m_nForceAtomType == 9 || m_nForceAtomType == e_ShadowBat)
	{
		oPacket.Encode4(rcStart.left);
		oPacket.Encode4(rcStart.top);
		oPacket.Encode4(rcStart.right);
		oPacket.Encode4(rcStart.bottom);
	}
	if (m_nForceAtomType == e_OrbitalFlame)
	{
		oPacket.Encode4((rcStart.left + rcStart.right) / 2);
		oPacket.Encode4((rcStart.top + rcStart.bottom) / 2);
	}
	if (m_nForceAtomType == 17)
	{
		oPacket.Encode4(m_nArriveDir);
		oPacket.Encode4(m_nArriveRange);
	}
	if (m_nForceAtomType == 18)
	{
		oPacket.Encode4(m_ptForcedTarget.x);
		oPacket.Encode4(m_ptForcedTarget.y);
	}
	if (m_nForceAtomType == e_LightningFusion || m_nForceAtomType == e_FireUmpire)
	{
		oPacket.Encode4(-120);
		oPacket.Encode4(100);
		oPacket.Encode4(120);
		oPacket.Encode4(100);
		oPacket.Encode4(0);
	}
	if (m_nForceAtomType == e_EnergeticExplosion || m_nForceAtomType == 18)
	{
		if (m_nForceAtomType == e_EnergeticExplosion)
		{
			oPacket.Encode4(rcStart.left);
			oPacket.Encode4(rcStart.top);
			oPacket.Encode4(rcStart.right);
			oPacket.Encode4(rcStart.bottom);
			oPacket.Encode4(m_nBulletItemID);
		}
		oPacket.Encode4(m_ptForcedTarget.x);
		oPacket.Encode4(m_ptForcedTarget.y);
	}

	pField->BroadcastPacket(&oPacket);
}
