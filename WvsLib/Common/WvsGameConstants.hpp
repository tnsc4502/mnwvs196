#pragma once
#include "..\WvsLib\Net\WorldInfo.h"

namespace WvsGameConstants
{
	static int nGamePort;

	static std::string strGameDesc;

	static long m_nEXP[251] = { 0 };

	static void LoadEXP() 
	{
		m_nEXP[1] = 15;
		m_nEXP[2] = 34;
		m_nEXP[3] = 57;
		m_nEXP[4] = 92;
		m_nEXP[5] = 135;
		m_nEXP[6] = 372;
		m_nEXP[7] = 560;
		m_nEXP[8] = 840;
		m_nEXP[9] = 1242;
		for (int i = 10; i < 200; i++) 
		{
			if (i >= 10 && i < 15
				|| i >= 30 && i < 35
				|| i >= 60 && i < 65
				|| i >= 100 && i < 105) 
			{
				m_nEXP[i] = m_nEXP[i - 1];
				continue;
			}
			m_nEXP[i] = (long)((double)m_nEXP[i - 1] * (i < 40 ? 1.2 : i < 75 ? 1.08 : i < 160 ? 1.07 : i < 200 ? 1.06 : 1));
		}

		for (int i = 200; i < 250; i++) 
		{
			if (i % 10 == 0) 
			{
				m_nEXP[i] = m_nEXP[i - 1] * 2;
				if (i != 200) 
					m_nEXP[i] = (long)((double)m_nEXP[i] * (i == 210 ? 1.06 : i == 220 ? 1.04 : i == 230 ? 1.02 : i == 240 ? 1.01 : 1));
				continue;
			}
			m_nEXP[i] = (long)((double)m_nEXP[i - 1] * (i < 210 ? 1.2 : i < 220 ? 1.06 : i < 230 ? 1.04 : i < 240 ? 1.02 : i < 250 ? 1.01 : 1));
		}
		m_nEXP[250] = 0;
	}


	static bool IsVSkillRootID(int nSkillID)
	{
		return nSkillID >= 40000 && nSkillID <= 40005;
	}

	static bool IsMakingSkillRecipe(int nSkillID)
	{
		int t = 10000 * (nSkillID / 10000);
		return !(nSkillID / 1000000 == 92 && !(nSkillID % 10000))
			&& (t / 1000000 == 92 && !(t % 10000));
	}

	static bool IsBeginnerJob(int a1)
	{
		if (a1 > 5000)
		{
			if (a1 >= 6000 && (a1 <= 6001 || a1 == 13000 || a1 == 14000))
				return 1;
		}
		else if (a1 == 5000 || a1 >= 2001 && (a1 <= 2005 || a1 > 3000 && (a1 <= 3002 || a1 > 4000 && a1 <= 4002)))
		{
			return 1;
		}
		if ((a1 >= 40000 && a1 <= 40005) || a1 % 1000 && a1 / 100 != 8000 && a1 != 8001 && !((a1 == 12000 || a1 == 12001 || a1 == 12002) || (a1 == 12003 || a1 == 12004)))
			return 0;
		return 1;
	}

	static int GetSkillRootFromSkill(int nSkillID)
	{
		int result = 0;

		result = nSkillID / 10000;
		if (nSkillID / 10000 == 8000)
			result = nSkillID / 100;
		return result;
	}

	static bool IsDualJob(int nJob)
	{
		return nJob % 10 == 43;
	}

	static bool IsCommonSkill(int nSkillID)
	{
		int v1; // eax@1

		v1 = nSkillID / 10000;
		if (nSkillID / 10000 == 8000)
			v1 = nSkillID / 100;
		return v1 >= 800000 && v1 <= 800099;
	}

	static bool IsNoviceSkill(int nSkillID)
	{
		int a1 = nSkillID;
		int v1; // esi@1
		bool result; // eax@2

		v1 = GetSkillRootFromSkill(a1);
		a1 = v1;
		if (IsVSkillRootID(v1))
			result = 0;
		else //sub_489B8B
		{
			if (a1 > 5000)
			{
				if (a1 >= 6000 && (a1 <= 6001 || a1 == 13000 || a1 == 14000))
					return true;
			}
			else if (a1 == 5000 || a1 >= 2001 && (a1 <= 2005 || a1 > 3000 && (a1 <= 3002 || a1 > 4000 && a1 <= 4002)))
			{
				return true;
			}
			if (IsVSkillRootID(a1)
				|| a1 % 1000
				&& a1 / 100 != 8000
				&& a1 != 8001
				&& !((a1 == 12000 || a1 == 12001 || a1 == 12002) || (a1 == 12003 || a1 == 12004))) //sub_488D12
				return false;
			return true;
		}
		return result;
	}

	static bool IsFieldAttackObjSkill(int nSkillID)
	{
		bool result; // al@3

		if (nSkillID && nSkillID >= 0)
			result = GetSkillRootFromSkill(nSkillID) == 9500;
		else
			result = 0;
		return result;
	}

	static bool IsEvanJob(int nJob)
	{
		int result; // eax@2

		switch (nJob)
		{
		case 2200:
		case 2210:
			result = 1;
			break;
		case 2211:
		case 2212:
		case 2213:
			result = 2;
			break;
		case 2214:
		case 2215:
		case 2216:
			result = 3;
			break;
		case 2217:
		case 2218:
			result = 4;
			break;
		default:
			result = 0;
			break;
		}
		return result > 0;
	}

	static int IsZeroJob(int a1)
	{
		return a1 == 10000 || a1 == 10100 || a1 == 10110 || a1 == 10111 || a1 == 10112;
	}

	static int GetEvanJobLevel(int a1)
	{
		signed int result; // eax@2

		switch (a1)
		{
		case 2200:
		case 2210:
			result = 1;
			break;
		case 2211:
		case 2212:
		case 2213:
			result = 2;
			break;
		case 2214:
		case 2215:
		case 2216:
			result = 3;
			break;
		case 2217:
		case 2218:
			result = 4;
			break;
		default:
			result = 0;
			break;
		}
		return result;
	}

	static int GetJobLevel(int nJob)
	{
		int result; // eax@6
		int v2; // esi@8

		if (IsBeginnerJob(nJob) || !(nJob % 100) || nJob == 501 || nJob == 3101)
		{
			result = 1;
		}
		else if (IsEvanJob(nJob))
		{
			result = GetEvanJobLevel(nJob);
		}
		else
		{
			if (IsDualJob(nJob))
				v2 = nJob % 10 / 2;
			else
				v2 = nJob % 10;
			result = (unsigned int)v2 <= 2 ? v2 + 2 : 0;
		}
		return result;
	}

	static bool IsIgnoreMasterLevel(int nSkillID)
	{
		int a1 = nSkillID;
		bool v1; // zf@11

		if (a1 <= (5320004 + 3))
		{
			if (a1 == (5320004 + 3))
				return 1;
			if (a1 <= 4210012)
			{
				if (a1 == 4210012
					|| a1 == 1120012
					|| a1 == 1320011
					|| a1 == 2121009
					|| a1 == 2221009
					|| a1 == 2321010
					|| a1 == 3210015)
					return 1;
				v1 = a1 == 4110012;
				goto LABEL_36;
			}
			if (a1 == 4340010 || a1 == 4340012)
				return 1;
			if (a1 > 5120010)
			{
				if (a1 <= 5120012 || a1 == 5220012 || a1 == 5220014)
					return 1;
				v1 = a1 == 5221022;
				goto LABEL_36;
			}
			return 0;
		}
		if (a1 > (23120010 + 1))
		{
			if (a1 == (23120010 + 3) 
				|| a1 == 23121008 
				|| a1 == 33120010
				|| a1 == 35120014
				|| a1 == 51120000)
				return 1;
			v1 = (a1 == 80001913);
		}
		else
		{
			if (a1 == (23120010 + 1))
				return 1;
			if (a1 > (21119969 + 4 * 11 + 1))
			{
				if (a1 < (21119969 + 4 * 12 + 3))
					return 0;
				if (a1 <= 21119969 + 4 * 13 || a1 == (21121004 + 4))
					return 1;
				v1 = a1 == 22171069;
			}
			else
			{
				if (a1 == (21119969 + 4 * 11 + 1) 
					|| a1 == (5321001 + 3) 
					|| a1 - (5321001 + 3) == 2)
					return 1;
				v1 = (a1 - (5321001 + 3) - 2 == (15799000 + 5));
			}
		}
	LABEL_36:
		if (!v1)
			return 0;
		return 1;
	}

	static bool IsAddedSPDualAndZeroSkill(int a1)
	{
		bool v1; // zf@9

		if (a1 > 101100101)
		{
			if (a1 == 101100201 || a1 == 101110102 || a1 == 101110200 || a1 == 101110203 || a1 == 101120104)
				return 1;
			v1 = a1 == 101120204;
		}
		else
		{
			if (a1 == 101100101
				|| a1 == 4311003
				|| a1 == 4321006
				|| a1 == 4330009
				|| a1 == 4331002
				|| a1 == 4340007
				|| a1 == 0x423D0C)
				return 1;
			v1 = a1 == 101000101;
		}
		if (!v1)
			return 0;
		return 1;
	}

	static bool IsSkillNeedMasterLevel(int nSkillID)
	{
		int v1, a1 = nSkillID; // edi@7
		int result; // eax@11

		result = 0;
		if (!IsIgnoreMasterLevel(a1)
			&& !(a1 / 1000000 == 92 && !(a1 % 10000))
			&& !IsMakingSkillRecipe(a1)
			&& !IsCommonSkill(a1)
			&& !IsNoviceSkill(a1)
			&& !IsFieldAttackObjSkill(a1))
		{
			v1 = GetSkillRootFromSkill(a1);
			if (!IsVSkillRootID(v1)
				&& a1 != 42120024
				&& !(v1 / 100 == 112 || v1 == 11000)
				&& (IsAddedSPDualAndZeroSkill(a1) || GetJobLevel(v1) == 4 && !IsZeroJob(v1)))
				result = 1;
		}
		return result > 0;
	}

	static bool IsKeyDownSkill(int skillid) 
	{
		int a1 = skillid;
		if (a1 == 3101008 || a1 == 1311011 || a1 == 2221011 || a1 == 2221052) 
			return true;
		
		if (a1 == 3111013 || a1 == 3121020 || a1 == 4341002)
			return true;

		if (a1 == 5221004)
			return true;

		if (a1 == 80001836 || a1 == 80001880)
			return true;

		if (a1 == 80001629 || a1 == 60011216 || a1 == 65121003 || a1 == 80001363)
			return true;
	
		if (a1 == 142111010 || a1 == 400041006)
			return true;

		if (a1 == 131001008 || a1 == 112001008 || a1 == 112110003 || a1 == 112111016)
			return true;

		/*if (a1 >= 101110101) {
		return true;
		}*/
		if (a1 == 80011366 || a1 == 80011051 || a1 == 80011112 || a1 == 80011116)
			return true;
	
		if (a1 >= 80001389 && a1 <= 80001392)
			return true;

		switch (skillid) 
		{
		case 0x226DB89:
		case 0x1F962F1:
		case 0x1F962F1 + 105:
		case 0x1F962F1 + 205:
		case 0x282B728:
		case 0x22729A8:
		case 0x22729A8 + 1000003:
		case 0x22729A8 + 1000052:
		case 0x1EA20AB:
		case 0x1DA9048:
		case 0x1DAB75D:
		case 0x1D909D8:
		case (0x17F5105 + 1):
		case 0x19D8812:
		case 0x19DAEBC:
		case (0x17F29DC + 1):
		case (0x1524DC9 + 2):
		case (0x1524DC9 + 2 + 949917):
		case (0x1524DC9 + 2 + 1949917):
		case 0xC835E9:
		case 0xD7511B + 3:
		case 131001000:
		case 131001001:
		case 131001002:
		case 131001003:
		case 131001101:
		case 131001102:
		case 131001103:
		case 131002000:
		case 131001012:
		case 131002012:
		case 0xD7782C:
		case 0xA9B19C:
		case 0xA9B19C + 3:
		case 0x574BE5:
		case 0x574BE5 + 3:
		case 5700010:
		case 5711021:
		case 41121001:
		case 35121015:
		case 27121201:
		case 1311011: // la mancha spear
		case 2221011: // freezing breath
		case 2221052: // lightning orb
		case 2321001: // big bang
		case 3101008: // covering fire
		case 3111013: // arrow blaster
		case 3121020: // hurricane
		case 4341002: // final cut
		case 5221004: // rapid fire
		case 5221022: // broadside
		case 5311002: // monkey wave
		case 60011216: // soul buster
		case 65121003: // soul resonance
		case 80001587: // airship lv. 1
		case 80001389: // ?
		case 80001390: // ?
		case 80001391: // ?
		case 80001392: // ?
		case 80001629: // ?
		case 80001836: // vanquisher's charm
		case 80001880: // liberate the rune of barrage
		case 80001887: // mille aiguilles
		case 95001001: // flying battle chair mount
		case 101110100: // wheel wind
		case 131001004: // let's roll!
		case 131001008: // sky jump
		case 142111010: // kinetic jaunt
			return true;
		default:
			return a1 == 95001001
				|| a1 == 80011362
				|| a1 == 131001004
				|| a1 == 400041009
				|| (a1 - 0x1F962F1 == 2000006)
				|| (a1 - 0x22729A8 == 5000001)
				|| a1 == 80001587
				|| a1 == 80001887
				|| a1 == 2321001
				|| a1 == 0x4D87AA
				|| a1 == 0x574BA9
				|| a1 - 0xA9B19C == 1000002
				|| a1 == 0x131CE0A
				|| a1 - (0x1524DC9 + 2) == 1949922
				|| a1 == 0x1CA1676
				|| a1 == 0x1DC3DF9;
		}
	}

	static bool IsSuperNovaSkill(int nSkillID)
	{
		switch (nSkillID)
		{
		case 4221052: // shadow veil
		case 65121052: // supreme supernova
			return true;
		default:
			return false;
		}
	}

	static bool IsRushBombSkill(int nSkillID)
	{
		switch (nSkillID) {
		case 2221012: // frozen orb
		case 5101012: // tornado uppercut
		case 12121001: // blazing extinction
		case 61111218: // wing beat
		case 101120200: // wind cutter
		case 101120203: // storm break
		case 101120205: // severe storm break
			return true;
		default:
			return false;
		}
	}

	static bool IsSpecialMeleeAttack(int nSkillID) 
	{
		switch (nSkillID)
		{
		case 101120200:
		case 101120203:
		case 101120205:
		case 61111100:
		case 61111113:
		case 61111218:
		case 31201001:
		case 14111022:
		case 22140015:
		case 22140024:
		case 12121001:
		case 5301001:
		case 5101014:
		case 5101012:
		case 5300007:
		case 27120211:
		case 14111023:
		case 400031010:
		case 400031003:
		case 400031004:
			return true;
		}
		return IsRushBombSkill(nSkillID);
	}

	static bool IsZeroSkill(int a1) 
	{
		int v1; // eax@1

		v1 = GetSkillRootFromSkill(a1);
		return IsZeroJob(v1) > 0;
	}

	static bool IsEvanForceSkill(int nSkillID)
	{
		switch (nSkillID) {
		case 22110022: // dragon flash
		case 22110023: // dragon flash
		case 22141011: // thunder circle
		case 22171062: // earth circle
		case 22140022: // dragon dive
		case 80001894: // dragon flash
			return true;
		default:
			return false;
		}
	}

	static bool IsUsingBulletMeleeAttack(int nSkillID) 
	{
		return (nSkillID == 14121052 
			|| (nSkillID >= 14111022 && nSkillID <= 14121003) 
			|| (nSkillID <= 14001027 && nSkillID >= 14000029));
	}

	static bool IsAdventurerWarrior(int nJob)
	{
		return nJob == 100
			|| nJob == 110
			|| nJob == 111
			|| nJob == 112
			|| nJob == 120
			|| nJob == 121
			|| nJob == 122
			|| nJob == 130
			|| nJob == 131
			|| nJob == 132;
	}

	static bool IsAdventurerMage(int nJob)
	{
		return nJob == 200
			|| nJob == 210
			|| nJob == 211
			|| nJob == 212
			|| nJob == 220
			|| nJob == 221
			|| nJob == 222
			|| nJob == 230
			|| nJob == 231
			|| nJob == 232;
	}	
	
	static bool IsAdventurerArchor(int nJob)
	{
		return nJob == 300 || nJob == 310 || nJob == 311 || nJob == 312 || nJob == 320 || nJob == 321 || nJob == 322;
	}

	static bool IsAdventurerRogue(int nJob)
	{
		return nJob == 400
			|| nJob == 420
			|| nJob == 421
			|| nJob == 422
			|| nJob == 410
			|| nJob == 411
			|| nJob == 412
			|| nJob / 10 == 43;
	}

	static bool IsCannonShooterJob(int nJob)
	{
		return nJob / 10 == 53 || nJob == 501;
	}

	static bool IsAdventurerPirate(int nJob)
	{
		bool result; // eax@8

		if (nJob == 500
			|| nJob == 510
			|| nJob == 511
			|| nJob == 512
			|| nJob == 520
			|| nJob == 521
			|| nJob == 522
			|| (result = IsCannonShooterJob(nJob)) != false)
			result = true;
		return result;
	}

	static bool IsCygnusJob(int nJob)
	{
		return nJob / 1000 == 1;
	}

	static bool IsResistanceJob(int nJob)
	{
		return nJob / 1000 == 3;
	}

	static bool IsXenonJob(int nJob)
	{
		return nJob / 100 == 36 || nJob == 3002;
	}

	static bool IsMercedesJob(int nJob)
	{
		return nJob / 100 == 23 || nJob == 2002;
	}

	static bool IsPhantomJob(int nJob)
	{
		return nJob / 100 == 24 || nJob == 2003;
	}

	static bool IsLeaderJob(int nJob)
	{
		return nJob / 1000 == 5;
	}

	static bool IsLuminousJob(int nJob)
	{
		return nJob / 100 == 27 || nJob == 2004;
	}

	static bool IsDragonBornJob(int nJob)
	{
		return nJob / 1000 == 6;
	}

	static bool IsHiddenJob(int nJob)
	{
		return nJob / 100 == 25 || nJob == 2005;
	}

	static bool IsAranJob(int nJob)
	{
		return nJob / 100 == 21 || nJob == 2000;
	}

	static bool IsKinesisJob(int nJob)
	{
		return nJob == 14000 || nJob == 14200 || nJob == 14210 || nJob == 14211 || nJob == 14212;
	}

	static bool IsJapanessJob(int nJob)
	{
		return nJob / 1000 == 4;
	}

	static bool IsKaiserJob(int nJob)
	{
		return nJob == 6000 || (nJob >= 6100 && nJob <= 6112);
	}

	static bool IsAngelicBusterJob(int nJob)
	{
		return nJob == 6001 || (nJob >= 6500 && nJob <= 6512);
	}

	static bool IsJettJob(int nJob)
	{
		return nJob / 10 == 57 || nJob == 508;
	}

	static bool IsBeastTamerJob(int nJob)
	{
		return nJob / 1000 == 11;
	}

	static bool IsPinkBeanJob(int nJob)
	{
		return nJob / 1000 == 13;
	}

	static bool IsDemonSlayerJob(int nJob)
	{
		return nJob / 100 == 31 || nJob ==  3001;
	}

	static bool IsExtendSPJob(int nJob)
	{
		bool result = false; // eax@18

		if (!nJob
			|| IsAdventurerWarrior(nJob)
			|| IsAdventurerMage(nJob)
			|| IsAdventurerArchor(nJob)
			|| IsAdventurerRogue(nJob)
			|| IsAdventurerPirate(nJob)
			|| IsCygnusJob(nJob)
			|| IsResistanceJob(nJob)
			|| IsEvanJob(nJob)
			|| IsMercedesJob(nJob)
			|| IsPhantomJob(nJob)
			|| IsLeaderJob(nJob)
			|| IsLuminousJob(nJob)
			|| IsDragonBornJob(nJob)
			|| IsZeroJob(nJob)
			|| IsHiddenJob(nJob)
			|| IsAranJob(nJob)
			|| IsJapanessJob(nJob)
			|| IsJettJob(nJob)
			|| (result = IsKinesisJob(nJob)) != false)
			result = true;
		return result;
	}
}