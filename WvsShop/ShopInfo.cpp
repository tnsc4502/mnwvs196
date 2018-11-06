#include "ShopInfo.h"
#include "..\WvsGame\ItemInfo.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\Database\GW_CashItemInfo.h"

ShopInfo::ShopInfo()
{
}


ShopInfo::~ShopInfo()
{
}

ShopInfo * ShopInfo::GetInstance()
{
	static ShopInfo* pInstance = new ShopInfo;
	return pInstance;
}

const CSCommodity * ShopInfo::GetCSCommodity(int nSN) const
{
	auto findIter = m_mCommodity.find(nSN);
	if (findIter == m_mCommodity.end())
		return nullptr;
	return &(findIter->second);
}

void ShopInfo::LoadCommodity(void * pCashPackage, bool bCheckValid)
{
	auto& refPackage = *((WZ::Node*)pCashPackage);
	for (auto& item : refPackage)
	{
		CSCommodity& commodity = m_mOriginalCommodity[(int)item["SN"]];
		commodity.nSN = (int)item["SN"];
		commodity.nItemID = (int)item["ItemId"];
		commodity.nCount = (int)item["Count"];
		commodity.nPrice = (int)item["Price"];
		commodity.nBonus = (int)item["Bonus"];
		commodity.nPeriod = (int)item["Period"];
		commodity.nPriority = (int)item["Priority"];
		commodity.nOnSale = (int)item["OnSale"];
		commodity.nForcedCatagory = (int)item["forcedCategory"];
		commodity.nForcedSubCategory = (int)item["forcedSubCategory"];
		commodity.nGameWorld = (int)item["gameWorld"];
		commodity.nPbCash = (int)item["PbCash"];
		commodity.nPbPoint = (int)item["PbPoint"];
		commodity.nPbGift = (int)item["PbGift"];
		commodity.nRefundable = (int)item["Refundable"];
		commodity.nWebShop = (int)item["WebShop"];
	}
	m_mCommodity = m_mOriginalCommodity;
}

GW_CashItemInfo * ShopInfo::GetCashItemInfo(const CSCommodity * pCS) const
{
	GW_CashItemInfo *pRet = AllocObj(GW_CashItemInfo);
	pRet->nItemID = pCS->nItemID;
	pRet->bRefundable = (pCS->nRefundable == 1);
	pRet->nNumber = pCS->nCount;
	pRet->nCommodityID = pCS->nSN;
	if (pCS->nPeriod)
	{
		if (ItemInfo::IsPet(pCS->nItemID))
			pRet->cashItemOption.ftExpireDate = GameDateTime::GetDateExpireFromPeriod(pCS->nPeriod);
		else
			pRet->liDateExpire = GameDateTime::GetDateExpireFromPeriod(pCS->nPeriod);
	}

	//pRet->nPaybackRate
	return pRet;
}

void ShopInfo::Init()
{
	auto& wzEtc = stWzResMan->GetWz(Wz::Etc);
	LoadCommodity((void*)&(wzEtc["Commodity"]), true);
}
