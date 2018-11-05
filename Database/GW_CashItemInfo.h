#pragma once
#include <string>
#include "..\WvsLib\DateTime\GameDateTime.h"

#include <vector>

class InPacket;
class OutPacket;

struct GW_CashItemInfo
{
	struct GW_CashItemOption
	{
		static const int OPTION_SIZE = 3;

		long long int liCashItemSN = 0, ftExpireDate = GameDateTime::TIME_PERMANENT;
		int nGrade = 0;
		int aOption[OPTION_SIZE] = { 0 };

		void Encode(OutPacket *oPacket);
		void Decode(InPacket *iPacket);
	};

	GW_CashItemOption cashItemOption;
	long long int liSN = 0,
				  liDateExpire = GameDateTime::TIME_UNLIMITED;

	double dDiscountRate;

	int nAccountID = 0,
		nCharacterID = 0,
		nItemID = 0,
		nCommodityID = 0,
		nPaybackRate = 0,
		nNumber = 0,
		nOrderNo = 0,
		nProductNo = 0,
		nSourceFlag = 0,
		nStoreBank = 0,
		nGWItemSlotInstanceType = 0;

	unsigned char bRefundable = false, bLocked = false;

	std::string sBuyCharacterID;

	GW_CashItemInfo();
	~GW_CashItemInfo();

	void Encode(OutPacket *oPacket);
	void Decode(InPacket *iPacket);

#ifdef DBLIB
	void Load(long long int liCashItemSN);
	void Save(bool bNewInstance = false);
#endif

	static std::vector<GW_CashItemInfo> LoadAll(int nAccountID, bool bLockedOnly = true);
};

