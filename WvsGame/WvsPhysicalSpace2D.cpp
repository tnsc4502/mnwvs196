#include "WvsPhysicalSpace2D.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "StaticFoothold.h"
#include "FootholdTree.h"
#include "AttrFoothold.h"

const FieldRect & WvsPhysicalSpace2D::GetRect() const
{
	return m_rcMBR;
}

bool WvsPhysicalSpace2D::IsPointInMBR(int x, int y, bool bAsClient)
{
	tagRECT rc;
	if (bAsClient)
	{
		SetRect(&rc, m_rcMBR.left + 9, m_rcMBR.top + 9, m_rcMBR.right - 9, m_rcMBR.bottom - 9);
	}
	else
	{
		rc.left = m_rcMBR.left;
		rc.top = m_rcMBR.top;
		rc.right = m_rcMBR.right;
		rc.bottom = m_rcMBR.bottom;
	}
	return PtInRect(&rc, POINT{ y, x }) == TRUE;
}

std::vector<FieldPoint> WvsPhysicalSpace2D::GetFootholdRandom(int nCount, FieldRect & rcRange)
{
	std::vector<FieldPoint> aRet;
	tagRECT rcRect, rcDst, rcMBR_;
	rcRect.left = rcRange.left;
	rcRect.top = rcRange.top;
	rcRect.right = rcRange.right;
	rcRect.bottom = rcRange.bottom;

	rcMBR_.left = m_rcMBR.left;
	rcMBR_.top = m_rcMBR.top;
	rcMBR_.right = m_rcMBR.right;
	rcMBR_.bottom = m_rcMBR.bottom;
	IntersectRect(&rcDst, &rcRect, &rcMBR_);

	int nXRange = (rcDst.right - rcDst.left + 1) / (2 * nCount), 
		nIdx = 0,
		nRndX = 0;
	auto anShuffle = Rand32::GetInstance()->GetRandomUniqueArray(0, 2 * nCount, 2 * nCount);
	if (nCount > 0)
	{
		while (aRet.size() != nCount)
		{
			nRndX = rcDst.left + (Rand32::GetInstance()->Random() % nXRange) + nXRange * anShuffle[nIdx++];
			auto lPosition = GetFootholdRange(
				nRndX,
				rcDst.top,
				rcDst.bottom
			);
			if (lPosition.size() > 0)
			{
				int nRnd = Rand32::GetInstance()->Random() % lPosition.size();
				FieldPoint pt;
				pt.x = nRndX;
				pt.y = (int)lPosition[nRnd];
				aRet.push_back(pt);
				if (aRet.size() == nCount)
					break;
			}
			if (nIdx >= 2 * nCount)
				break;
		}
	}
	return aRet;
}

std::vector<long long int> WvsPhysicalSpace2D::GetFootholdRange(int x, int y1, int y2)
{
	std::vector<long long int> lPosition;
	auto lRes = m_pTree->Search({ x - 1, y1 - 1 }, { x + 1, y2 + 1 });
	for (auto& res : lRes)
	{
		if (res->m_ptPos1.x < res->m_ptPos2.x
			&& res->m_ptPos1.x < x
			&& res->m_ptPos2.x > x)
		{
			long long int liDst =
				res->m_ptPos1.y + (x - res->m_ptPos1.x) * (res->m_ptPos2.y - res->m_ptPos1.y) / (res->m_ptPos2.x - res->m_ptPos1.x);
			if (liDst >= y1 && y2 >= liDst)
				lPosition.push_back(liDst);
		}
	}
	return lPosition;
}

StaticFoothold * WvsPhysicalSpace2D::GetFootholdClosest(Field * pField, int x, int y, int * pcx, int * pcy, int ptHitx)
{
	StaticFoothold *pRet = nullptr;

	int nXOffset = 0, nYOffset = 0, nDist = 0, nMin = 0x7fffffff;
	for (int i = 0; i < m_lFoothold.size(); ++i)
	{
		auto pFh = m_lFoothold[i];
		if (pFh->m_ptPos1.x >= pFh->m_ptPos2.x)
			continue;
		int nOF = __OFSUB__(ptHitx, x);
		int nCmp = ptHitx - x < 0;
		//nXOffset = 0, nYOffset = 0, nDist = 0, nMin = 0x7fffffff;
		if (ptHitx > x && pFh->m_ptPos1.x < x)
			continue;
		if ((!(nCmp ^ nOF) || pFh->m_ptPos2.x <= x) && (pFh->m_ptPos1.y >= y - 100))
		{
			if (pFh->m_ptPos2.y >= y - 100)
			{
				if (ptHitx <= x)
				{
					if (ptHitx == x) 
					{
						nXOffset = (pFh->m_ptPos1.x + pFh->m_ptPos2.x) / 2 - x;
						nYOffset = (pFh->m_ptPos1.y + pFh->m_ptPos2.y) / 2 - y;
					}
					else
					{
						nXOffset = pFh->m_ptPos2.x - x;
						nYOffset = pFh->m_ptPos2.y - y;
					}
				}
				else
				{
					nXOffset = pFh->m_ptPos1.x - x;
					nYOffset = pFh->m_ptPos1.y - y;
				}
				nDist = nXOffset * nXOffset + (nYOffset) * (nYOffset);
				if (nDist < nMin)
				{
					int nX1 = pFh->m_ptPos1.x;
					int nX2 = pFh->m_ptPos2.x;
					if (x > nX1 && (x >= nX2 || x - (nX1 + nX2) / 2 >= 0))
						nX1 = nX2;
					int nY = pFh->m_ptPos1.y + (pFh->m_ptPos2.y - pFh->m_ptPos1.y) * (nX1 - pFh->m_ptPos1.x) / (nX2 - pFh->m_ptPos1.x);
					int nLeft = m_rcMBR.left + 10;
					if (nX1 <= nLeft || (nLeft = m_rcMBR.right - 10, nX1 >= nLeft))
						nX1 = nLeft;

					*pcx = nX1;
					*pcy = nY;
					nMin = nDist;
					pRet = pFh;
				}
			}
		}
	}
	if (!pRet)
	{
		pRet = GetFootholdUnderneath(x, y - 100, pcy);
		if (x <= m_rcMBR.left + 10)
			x = m_rcMBR.left + 10;
		else if (x >= m_rcMBR.right - 10)
			x = m_rcMBR.right - 10;
		*pcx = x;
		if (!pRet)
		{
			for (auto& p : m_lFoothold)
			{
				int nX1 = p->m_ptPos1.x, nX2 = p->m_ptPos2.x, nY1 = p->m_ptPos1.y, nY2 = p->m_ptPos2.y;
				if (nX1 < nX2)
				{
					int nX = 0;
					nDist = (int)(std::pow(((nX1 + nX2) / 2) - x, 2) + std::pow(((nY1 + nY2) / 2) - y, 2));
					if (nDist < nMin)
					{
						if (x > nX1)
						{
							if (x < nX2)
								nX = (x - (nX1 + nX2) / 2) < 0 ? nX1 : nX2;
							else
								nX = nX2;
						}
						else
							nX = nX1;
						auto nD = (p->m_ptPos1.y) + (nX - nX1) * (p->m_ptPos2.y - p->m_ptPos1.y) / (nX2 - nX1);
						int nLeft = m_rcMBR.left + 10;
						if (nX <= nLeft || (nLeft = m_rcMBR.right - 10, nX >= nLeft))
							nX = nLeft;

						*pcx = nX;
						*pcy = nD;
						nMin = nDist;
						pRet = p;
					}
				}
			}
		}
	}
	return pRet;
}

StaticFoothold * WvsPhysicalSpace2D::GetFootholdUnderneath(int x, int y, int * pcy)
{
	auto lRes = m_pTree->Search(
		{ x - 1, y - 1 },
		{ x + 1, 0x7fffffff }
	);
	StaticFoothold* pRet = nullptr;
	int nMin = 0x7fffffff, nX1, nX2, nOffset;
	for (auto& pRes : lRes)
	{
		nX1 = pRes->m_ptPos1.x;
		nX2 = pRes->m_ptPos2.x;
		if (nX1 < nX2 && nX1 <= x && nX2 >= x)
		{
			nOffset = (pRes->m_ptPos1.y) + (x - nX1) * ((pRes->m_ptPos2.y - pRes->m_ptPos1.y) / (nX2 - nX1));
			if (nOffset >= y && nOffset < nMin)
			{
				nMin = nOffset;
				*pcy = nMin;
				pRet = pRes;
			}
		}
	}
	return pRet;
}

void WvsPhysicalSpace2D::Load(void * pPropFoothold, void * pLadderRope, void * pInfo)
{
	auto& refPropFoothold = *((WZ::Node*)pPropFoothold);
	auto& refLadderRope = *((WZ::Node*)pLadderRope);
	auto& refInfo = *((WZ::Node*)pInfo);

	StaticFoothold *pFoothold = nullptr;
	long long int lPage = 0, lZMass = 0;
	for (auto& page : refPropFoothold)
	{
		lPage = atoll(page.Name().c_str());
		for (auto& zMass : page)
		{
			lZMass = atoll(zMass.Name().c_str());
			auto massIter = m_mMassRange.find(lZMass);
			if (massIter == m_mMassRange.end()) 
			{
				auto insertRes = m_mMassRange.insert({ lZMass, {0, 0} });
				massIter = insertRes.first;
			}
			for (auto& foothold : zMass)
			{
				pFoothold = AllocObj(StaticFoothold);
				pFoothold->m_pAttrFoothold = AllocObj(AttrFoothold);
				pFoothold->m_pAttrFoothold->m_dDrag = (double)((int)foothold["drag"]) * 0.01;
				pFoothold->m_pAttrFoothold->m_dForce = (double)((int)foothold["force"]) * 0.01;
				pFoothold->m_nSN = atoi(foothold.Name().c_str());
				pFoothold->m_lPage = lPage;
				pFoothold->m_lZMass = lZMass;
				pFoothold->m_ptPos1.x = (int)foothold["x1"];
				pFoothold->m_ptPos1.y = (int)foothold["y1"];
				pFoothold->m_ptPos2.x = (int)foothold["x2"];
				pFoothold->m_ptPos2.y = (int)foothold["y2"];
				pFoothold->m_nSNPrev = (int)foothold["prev"];
				pFoothold->m_nSNNext = (int)foothold["next"];
				pFoothold->ValidateVectorInfo();
				m_mFoothold.insert({ pFoothold->m_nSN, pFoothold });
				m_lFoothold.push_back(pFoothold);
				m_pTree->InsertData(
					pFoothold->m_ptPos1,
					pFoothold->m_ptPos2,
					pFoothold
				);
				if (pFoothold->m_ptPos1.y == pFoothold->m_ptPos2.y && !pFoothold->m_pAttrFoothold->m_dForce)
					m_lFootholdFixedMob.push_back(pFoothold);
				
				int nRight = pFoothold->m_ptPos1.x, nLeft = pFoothold->m_ptPos1.x;
				int nTop = pFoothold->m_ptPos2.y, nBottom = pFoothold->m_ptPos2.y;

				if (pFoothold->m_ptPos1.x >= pFoothold->m_ptPos2.x)
					nLeft = pFoothold->m_ptPos2.x;
				else
				{
					nLeft = pFoothold->m_ptPos1.x;
					nRight = pFoothold->m_ptPos2.x;
				}

				if (pFoothold->m_ptPos1.y >= pFoothold->m_ptPos2.y)
				{
					nBottom = pFoothold->m_ptPos1.y;
					nTop = pFoothold->m_ptPos2.y;
				}
				else
					nTop = pFoothold->m_ptPos1.y;

				if (m_rcMBR.left > nLeft + 30)
					m_rcMBR.left = nLeft + 30;
				if (massIter->second.first > nLeft)
					massIter->second.first = nLeft;

				if (m_rcMBR.right < nRight - 30)
					m_rcMBR.right = nRight - 30;
				if (massIter->second.second < nLeft)
					massIter->second.second = nLeft;
				if (m_rcMBR.top > nTop)
					m_rcMBR.top = nTop;
				if (m_rcMBR.bottom < nBottom + 10)
					m_rcMBR.bottom = nBottom + 10;
			}
		}
	}
	if (pInfo)
	{
		auto& refInfo = *((WZ::Node*)pInfo);
		int nLeft = (int)refInfo["VRLeft"],
			nRight = (int)refInfo["VRLeft"],
			nTop = (int)refInfo["VRTop"],
			nBottom = (int)refInfo["VRBottom"];
		if (nLeft && m_rcMBR.left < nLeft)
			m_rcMBR.left = nLeft;
		if (nRight && m_rcMBR.right < nRight - 20)
			m_rcMBR.right = nRight - 20;
		if (nTop && m_rcMBR.top < nTop + 65)
			m_rcMBR.top = nTop + 65;
		if (nBottom && m_rcMBR.bottom + nBottom)
			m_rcMBR.bottom = nBottom;
	}

	//Inflate
	m_rcMBR.left -= 10;
	m_rcMBR.top -= 10;
	m_rcMBR.right += 10;
	m_rcMBR.bottom += 10;
}

WvsPhysicalSpace2D::WvsPhysicalSpace2D()
{
	m_pTree = AllocObj(FootholdTree);
	m_rcMBR.left = 0x7FFFFFFF;
	m_rcMBR.top = 0x7FFFFFFF;
	m_rcMBR.right = -1;
	m_rcMBR.bottom = -1;
}


WvsPhysicalSpace2D::~WvsPhysicalSpace2D()
{
}
