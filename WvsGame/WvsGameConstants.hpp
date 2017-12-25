#pragma once
#include "Net\WorldInfo.h"

namespace WvsGameConstants
{
	static int nGamePort;

	static std::string strGameDesc;

	static WorldConnectionInfo CenterServerList[1] =
	{
		WorldConnectionInfo{ "127.0.0.1", 8383 }
	};

	static bool IsVSkill(int nSkillID)
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
		return nJob / 100 == 22 || nJob == 2001;
	}

	static bool IsCommonSkill(int nSkillID)
	{
		int v1; // eax@1
		bool result; // eax@2

		v1 = GetSkillRootFromSkill(nSkillID);
		if ((v1 - 800000) > 0x63)
			result = v1 == 8001;
		else
			result = 1;
		return result;
	}

	static bool IsNoviceSkill(int nSkillID)
	{
		int a1 = nSkillID;
		int v1; // esi@1
		bool result; // eax@2

		v1 = GetSkillRootFromSkill(a1);
		if (IsVSkill(v1))
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
			if (IsVSkill(a1)
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

	static int GetEvenJobLevel(int nJob)
	{
		return nJob / 10 == 43;
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
			result = GetEvenJobLevel(nJob);
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

		if (a1 <= (0x512D44 + 3))
		{
			if (a1 == (0x512D44 + 3))
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
			if (a1 == 4340010 || a1 == 0x42392C)
				return 1;
			if (a1 > 0x4E200A)
			{
				if (a1 <= 5120012 || a1 == 0x4FA6AC || a1 == 5220014)
					return 1;
				v1 = a1 == 5221022;
				goto LABEL_36;
			}
			return 0;
		}
		if (a1 > (0x160C88A + 1))
		{
			if (a1 == (0x160C88A + 3) || a1 == 0x160CC70 || a1 == 0x1F95F0A || a1 == 0x217E38E || a1 == 51120000)
				return 1;
			v1 = a1 == 80001913;
		}
		else
		{
			if (a1 == (0x160C88A + 1))
				return 1;
			if (a1 > (0x14243E1 + 4 * 11 + 1))
			{
				if (a1 < (0x14243E1 + 4 * 12 + 3))
					return 0;
				if (a1 <= 0x14243E1 + 4 * 13 || a1 == (0x14247EC + 4))
					return 1;
				v1 = a1 == 0x1524DBD;
			}
			else
			{
				if (a1 == (0x14243E1 + 4 * 11 + 1) || a1 == (0x513129 + 3) || a1 - (0x513129 + 3) == 2)
					return 1;
				v1 = a1 - (0x513129 + 3) - 2 == (0xF112D8 + 5);
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
			if (!IsVSkill(v1)
				&& a1 != 0x282B358
				&& !(v1 / 100 == 112 || v1 == 11000)
				&& (IsAddedSPDualAndZeroSkill(a1) || GetJobLevel(v1) == 4 && !IsZeroJob(v1)))
				result = 1;
		}
		return result > 0;
	}
}