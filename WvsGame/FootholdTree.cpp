#include "FootholdTree.h"
#include "WvsPhysicalSpace2D.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

FootholdTree::FootholdTree()
{
	m_aNode.push_back(AllocObj( NODE ));
	m_pRoot = m_aNode[m_aNode.size() - 1];
	m_pRoot->nLevel = m_pRoot->nCount = 0;
}

void FootholdTree::raw_AdjustTree(NODE * pNode, I2 * pi)
{
	if (pNode != m_pRoot)
	{
		NODE *pIter = nullptr;
		do
		{
			pIter = pNode->pParent;
			auto pFind = pIter->FindNodeEntry(pNode);
			if (pi)
			{
				if (!pNode->bValidMBR)
					pNode->RecalcMBR();
				pNode->iMBR.Union(*pi);
			}
			else
				pNode->RecalcMBR();
			if (!pNode->bValidMBR)
				pNode->RecalcMBR();

			pFind->i = pNode->iMBR;
			pNode = pIter;
		} while (pIter != m_pRoot);
	}
}

FootholdTree::NODE * FootholdTree::raw_ChooseSubTree(I2 * i, int nLevel)
{
	NODE* pResult = m_pRoot, *pSelected = nullptr;
	int nDiff = 0, nDiffSelected = 0, nFirstOverlap = 0, nFirstOverlapSelected = 0, nVal = 0, nOffset = 0;
	for (int j = pResult->nLevel; j != nLevel; j = pResult->nLevel)
	{
		pSelected = nullptr;
		if (j == 1)
		{
			for (int idx = 0; idx < pResult->nCount; ++idx)
			{
				nOffset = 0;
				I2 tmp = pResult->E[idx].i;
				tmp.Union(*i);
				nVal = 0;
				for (int idx_ = 0; idx_ < pResult->nCount; ++idx_)
					if (idx != idx_)
					{
						I2 tmp2 = pResult->E[idx].i;
						if (tmp2.Overlap(&(pResult->E[idx_].i)))
							nVal += (tmp2.r - tmp2.l) * (tmp2.b - tmp2.t);
						tmp2 = tmp;
						if (tmp2.Overlap(&(pResult->E[idx_].i)))
							nOffset += (tmp2.r - tmp2.l) * (tmp2.b - tmp2.t);
					}
				nDiff = nOffset - nVal;
				nFirstOverlap = (tmp.r - tmp.l) * (tmp.b - tmp.t)
					- (pResult->E[idx].i.r - pResult->E[idx].i.l) * (pResult->E[idx].i.b - pResult->E[idx].i.t);

				if (!pSelected || nDiff < nDiffSelected || (nDiff == nDiffSelected && nFirstOverlap < nFirstOverlapSelected))
				{
					pSelected = pResult->E[idx].pChild;
					nDiffSelected = nDiff;
					nFirstOverlapSelected = nFirstOverlap;
				}
			}
		}
		else
		{
			int nV1 = 0, nV2 = 0, nV3 = 0;
			for (int idx = 0; idx < pResult->nCount; ++idx)
			{
				I2 tmp = pResult->E[idx].i;
				tmp.Union(*i);
				nV1 = (pResult->E[idx].i.r - pResult->E[idx].i.l) * (pResult->E[idx].i.b - pResult->E[idx].i.t);
				nV2 = (tmp.r - tmp.l) * (tmp.b - tmp.t);
				nV3 = nV2 - nV1;
				if (!pSelected || nV2 < nDiffSelected || (nV2 == nDiffSelected && nV3 < nFirstOverlapSelected))
				{
					pSelected = pResult->E[idx].pChild;
					nDiffSelected = nV2;
					nFirstOverlapSelected = nV3;
				}
			}
		}
		pResult = pSelected;
	}
	return pResult;
}

void FootholdTree::raw_Insert(NODE::ENTRY * e, std::vector<int>& lLev, int nLevel)
{
	auto pTree = raw_ChooseSubTree(&(e->i), nLevel);
	pTree->InsertEntry(e);
	raw_AdjustTree(pTree, &(e->i));
	NODE *pNode = nullptr;
	if (pTree->nCount > NODE::MAX_ENTRY - 1)
	{
		while (true)
		{
			pNode = raw_OverflowTreatment(pTree, lLev);
			if (!pNode)
				break;
			if (pTree == m_pRoot)
			{
				m_aNode.push_back(AllocObj( NODE ));
				auto& ref = *m_aNode[m_aNode.size() - 1];
				ref.nLevel = pTree->nLevel + 1;
				m_pRoot = &ref;
				NODE::ENTRY newEntry;
				if (!pTree->bValidMBR)
					pTree->RecalcMBR();
				newEntry.i = pTree->iMBR;
				newEntry.pChild = pTree;
				m_pRoot->InsertEntry(&newEntry);
				if (!pNode->bValidMBR)
					pNode->RecalcMBR();
				newEntry.i = pNode->iMBR;
				newEntry.pChild = pNode;
				m_pRoot->InsertEntry(&newEntry);
				return;
			}
			auto pParent = pTree->pParent;
			NODE::ENTRY* pSelected = nullptr;
			for (int idx = 0; idx < pParent->nCount; ++idx)
			{
				if (pParent->E[idx].pChild == pTree)
				{
					pSelected = &(pParent->E[idx]);
					break;
				}
			}
			if (!pTree->bValidMBR)
				pTree->RecalcMBR();
			pSelected->i = pTree->iMBR;
			pTree = pParent;
			if (!pNode->bValidMBR)
				pNode->RecalcMBR();
			NODE::ENTRY newEntry;
			newEntry.i = pNode->iMBR;
			newEntry.pChild = pNode;
			pParent->InsertEntry(&newEntry);
			if (pParent->nCount <= 4)
				return;
		}
	}
}

void FootholdTree::raw_Reinsert(NODE * pNode, std::vector<int>& lLev)
{
#define HIDWORD(_qw)    ((int)(((_qw) >> 32) & 0xffffffff))
	if (!pNode->bValidMBR)
		pNode->RecalcMBR();
	auto il = pNode->iMBR.l, it = pNode->iMBR.t, ir = pNode->iMBR.r, ib = pNode->iMBR.b;
	auto nHeightAvg = (it + ib) / 2;
	auto nWidthAvg = (ir + il) / 2;
	int nWAvg = 0, nHAvg = 0;
	std::pair<int, int> aCalc[NODE::MAX_ENTRY];
	NODE::ENTRY aBackUp[NODE::MAX_ENTRY];

	for (int idx = 0; idx < NODE::MAX_ENTRY; ++idx)
	{
		aCalc[idx].first = idx;
		nWAvg = (pNode->E[idx].i.l + pNode->E[idx].i.r);
		nHAvg = (pNode->E[idx].i.b + pNode->E[idx].i.t);
		aCalc[idx].second = (int)(std::pow(nWAvg / 2 - nWidthAvg, 2)
			+ std::pow((nHAvg /*- HIDWORD(nHAvg)*/) / 2 - nHeightAvg, 2));
	}

	std::sort(aCalc, aCalc + NODE::MAX_ENTRY, [&](std::pair<int, int>& p1, std::pair<int, int>&p2) {
		return p1.second < p2.second;
	});

	int nCount = 1, nIdx = 0;
	do
	{
		aBackUp[nIdx] = pNode->E[aCalc[nIdx].first];
		pNode->DeleteEntry(aCalc[nIdx].first);
		int nTmp = nCount;
		if (nCount < 2)
		{
			int j = nIdx + 1;
			while (aCalc[j].first != pNode->nCount)
			{
				++nTmp;
				++j;
				if (nTmp >= 2)
					goto OUT__;
			}
			aCalc[nTmp] = aCalc[nIdx];
		}
	OUT__:
		++nIdx;
		++nCount;
	} while (nCount - 1 < 2);

	raw_AdjustTree(pNode, nullptr);
	int nLevel = pNode->nLevel;
	for (int idx = nIdx - 1; idx >= 0; --idx)
		raw_Insert(&aBackUp[idx], lLev, nLevel);

#undef HIDWORD
}

FootholdTree::NODE * FootholdTree::raw_SplitNode(NODE * pNode)
{
	I2 aSet1[4], aSet2[4];
	NODE::ENTRY aBackUp[NODE::MAX_ENTRY * 2];
	int nCount = 1, nLooped = 0, nVal = 0;
	do
	{
		int v54[NODE::MAX_ENTRY];
		for (int i = 0; i < NODE::MAX_ENTRY; ++i)
			v54[i] = i;
		nCount = 1;
		//int nIdx1 = 0, nIdx2 = 0;
		int *v4 = &v54[0];
		int *i2__ = &v54[0];
		do
		{
			if (nCount < NODE::MAX_ENTRY)
			{
				int v10 = 0, v11 = 0, v9 = 0, v14 = 0;
				int *v64 = v4 + 1;
				int nRemained = NODE::MAX_ENTRY - nCount;
				do
				{
					int nTmp = *v4;
					auto pE1 = pNode->E[*v4];
					auto pE2 = pNode->E[*v64];
					if (nLooped)
					{
						if (pE1.i.t > pE2.i.t)
							goto L15;
						if (pE1.i.t != pE2.i.t)
							goto L16;
						v14 = pE1.i.b;
						v11 = WvsPhysicalSpace2D::__OFSUB__(v14, pE2.i.b);
						v9 = (v14 == pE2.i.b);
						v10 = (v14 - pE2.i.b < 0);
					}
					else
					{
						if (pE1.i.l > pE2.i.l)
							goto L15;
						if (pE1.i.l != pE2.i.l)
							goto L16;
						v14 = pE1.i.r;
						v11 = WvsPhysicalSpace2D::__OFSUB__(v14, pE2.i.r);
						v9 = (v14 == pE2.i.r);
						v10 = (v14 - pE2.i.r < 0);
					}
					if ((unsigned char)(v10 ^ v11) | v9)
					{
					L16:
						//nIdx1 = nIdx2;
						v4 = i2__;
						goto L17;
					}
				L15:
					//nIdx2 = nIdx3;
					v4 = i2__;
					*i2__ = *v64;
					*v64 = nTmp;
					//nIdx1 = nIdx2;
					//nIdx3 = nTmp;
				L17:
					++v64;
					--nRemained;
				} while (nRemained > 0);
			}
			++nCount;
			++v4;
			i2__ = v4;
			//nIdx2 = nIdx1;
		} while (nCount - 1 < NODE::MAX_ENTRY);

		for (int idx = 0; idx < NODE::MAX_ENTRY; ++idx)
			aBackUp[idx + (nLooped * NODE::MAX_ENTRY)] = pNode->E[idx];

		I2 i1 = aBackUp[nLooped * NODE::MAX_ENTRY].i;
		I2 i2 = aBackUp[nLooped * NODE::MAX_ENTRY + 1].i;

		I2 v55 = aBackUp[nLooped * NODE::MAX_ENTRY + 4].i;
		I2 v64 = aBackUp[nLooped * NODE::MAX_ENTRY + 3].i;

		int j = 1;
		do
		{
			i1.Union(i2);
			v55.Union(v64);
			aSet1[j - 1 + nLooped * 2] = i1;
			aSet2[j - 1 + nLooped * 2] = v55;
			i2 = aBackUp[nLooped * NODE::MAX_ENTRY + 1 + j].i;
			v64 = aBackUp[nLooped * NODE::MAX_ENTRY + 3 - j].i;
			++j;
		} while (j < 3);
		++nLooped;
	} while (nLooped < 2);
	nCount = 0;
	int nTotal[2] = { 0 };
	for (int j = 0; j < 2; ++j)
	{
		for (int k = 0; k < 2; ++k)
		{
			auto i1 = aSet1[(j * 2) + k];
			//20 + 8 + 4 = 4 * 4 * 2
			auto i2 = aSet2[(j * 2) + 1 - k];
			nVal = i2.b + i1.b + i2.r + i1.r;
			nVal = i2.t - i1.t - i2.l - i1.l;
			nTotal[j] += nVal;
		}
	}

	bool v60 = nTotal[0] > nTotal[1];

	I2 v57;
	int v64 = 0, v63 = -1, v32, v33, i2_ = 0, v61 = 0;
	for (int j = 0; j < 2; ++j)
	{
		I2 v30 = aSet1[2 * (v60 ? 1 : 0) + j];
		I2 v31 = aSet2[1 + 2 * (v60 ? 1 : 0) - j];

		v57 = v30;
		if (v57.Overlap(&v31))
			v64 = (v57.r - v57.l) * (v57.b - v57.t);
		else
			v64 = 0;
		v32 = v63;
		v33 = (v30.r - v30.l) * (v30.b - v30.t) - (v31.r - v31.l) * (v31.b - v31.t);
		if (v63 == -1 || v64 < i2_ || (v64 == i2_) && (v33 < v61))
		{
			v32 = j + 2;
			v63 = j + 2;
			i2_ = v64;
			v61 = v33;
		}
	}

	int nLevel = pNode->nLevel;
	m_aNode.push_back(AllocObj( NODE ));
	auto& refNode = *m_aNode[m_aNode.size() - 1];
	refNode.nLevel = nLevel;
	pNode->RemoveAllEntries();
	if (v32 > 0)
	{
		v63 = v32;
		do
		{
			pNode->InsertEntry(&aBackUp[NODE::MAX_ENTRY * (v60 ? 1 : 0) + (v32 - v63)]);
			--v63;
		} while (v63);
	}
	if (v32 < 5)
	{
		int v40 = 5 - v32;
		do
		{
			refNode.InsertEntry(&aBackUp[v32 + (v60 ? 1 : 0) * NODE::MAX_ENTRY + (5 - v32) - v40]);
			--v40;
		} while (v40);
	}
	return &refNode;
}

void FootholdTree::InsertData(const FieldPoint & pt0, const FieldPoint & pt1, StaticFoothold * pData)
{
	NODE::ENTRY e;
	if (pt0.x >= pt1.x)
	{
		e.i.l = pt1.x;
		e.i.r = pt0.x;
	}
	else
	{
		e.i.l = pt0.x;
		e.i.r = pt1.x;
	}
	if (pt0.y >= pt1.y)
	{
		e.i.t = pt1.y;
		e.i.b = pt0.y;
	}
	else
	{
		e.i.t = pt0.y;
		e.i.b = pt1.y;
	}
	e.pChild = nullptr;
	e.pt1 = pt0;
	e.pt2 = pt1;
	e.pS = pData;
	std::vector<int> lLev;
	raw_Insert(&e, lLev, 0);
}

FootholdTree::NODE * FootholdTree::raw_OverflowTreatment(NODE * pNode, std::vector<int>& lLev)
{
	NODE* pSplit = nullptr;
	auto findIter = lLev.end();
	if (!(pNode == m_pRoot) &&
		(findIter = std::find(lLev.begin(), lLev.end(), pNode->nLevel), findIter == lLev.end()))
	{
		lLev.push_back(pNode->nLevel);
		raw_Reinsert(pNode, lLev);
	}
	else
		pSplit = raw_SplitNode(pNode);

	return pSplit;
}

void FootholdTree::raw_Search(NODE * pNode, I2 * i, std::vector<StaticFoothold*>& lRes)
{
	if (pNode->nLevel)
	{
		for (int idx = 0; idx < pNode->nCount; ++idx)
		{
			if (pNode->E[idx].i.r >= i->l && pNode->E[idx].i.b >= i->t
				&& pNode->E[idx].i.l <= i->r && pNode->E[idx].i.t <= i->b)
				raw_Search(pNode->E[idx].pChild, i, lRes);
		}
	}
	else
	{
		for (int idx = 0; idx < pNode->nCount; ++idx)
		{
			if (pNode->E[idx].i.r >= i->l && pNode->E[idx].i.b >= i->t
				&& pNode->E[idx].i.l <= i->r && pNode->E[idx].i.t <= i->b)
			{
				auto findIter = std::find(lRes.begin(), lRes.end(), pNode->E[idx].pS);
				if (findIter == lRes.end())
					lRes.push_back(pNode->E[idx].pS);
			}
		}
	}
}

std::vector<StaticFoothold*> FootholdTree::Search(const FieldPoint & pt0, const FieldPoint & pt1)
{
	std::vector<StaticFoothold*> ret;
	I2 i;
	if (pt0.x >= pt1.x)
	{
		i.l = pt1.x;
		i.r = pt0.x;
	}
	else
	{
		i.l = pt0.x;
		i.r = pt1.x;
	}
	if (pt0.y >= pt1.y)
	{
		i.t = pt1.y;
		i.b = pt0.y;
	}
	else
	{
		i.t = pt0.y;
		i.b = pt1.y;
	}
	raw_Search(m_pRoot, &i, ret);
	return ret;
}

void FootholdTree::NODE::InsertEntry(ENTRY * e)
{
	//if (!e->pChild)
	//	return;
	E[nCount++] = *e;
	if (nLevel > 0)
		e->pChild->pParent = this;
	if (nCount == 1)
	{
		bValidMBR = true;
		iMBR = e->i;
	}
	else if (bValidMBR)
		iMBR.Union(e->i);
}

void FootholdTree::NODE::RecalcMBR()
{
	iMBR = E[0].i;
	for (int i = 1; i < nCount; ++i)
		iMBR.Union(E[i].i);
	bValidMBR = true;
}

void FootholdTree::NODE::RemoveAllEntries()
{
	nCount = 0;
	bValidMBR = false;
}

void FootholdTree::NODE::DeleteEntry(int nIdx)
{
	for (int i = nIdx; i < nCount - 1; ++i)
		E[i] = E[i + 1];
	--nCount;
	bValidMBR = false;
}

FootholdTree::NODE::ENTRY * FootholdTree::NODE::FindNodeEntry(NODE * pNode)
{
	for (int i = 0; i < nCount; ++i)
		if (E[i].pChild == pNode)
			return &E[i];
	return nullptr;
}

bool FootholdTree::I2::Overlap(I2 * i2)
{
	bool result = false;
	int v3, v5, v6, v7; // eax@12

	v3 = i2->l;
	if (this->r >= i2->l && this->b >= i2->t && this->l <= i2->r && this->t <= i2->b)
	{
		if (v3 < this->l)
			v3 = this->l;
		this->l = v3;
		v5 = i2->t;
		if (v5 < this->t)
			v5 = this->t;
		this->t = v5;
		v6 = i2->r;
		if (this->r < v6)
			v6 = this->r;
		this->r = v6;
		v7 = this->b;
		if (v7 >= i2->b)
			v7 = i2->b;
		this->b = v7;
		result = true;
	}
	return result;
}

FootholdTree::I2 & FootholdTree::I2::Union(I2 & i2)
{
	I2 *i1 = this;
	int v3, v4, v5, v6;

	v3 = i2.l;
	if (i1->l < v3)
		v3 = i1->l;
	this->l = v3;
	v4 = i2.t;
	if (i1->t < v4)
		v4 = i1->t;
	this->t = v4;
	v5 = i2.r;
	if (v5 < i1->r)
		v5 = i1->r;
	this->r = v5;
	v6 = i2.b;
	if (v6 < i1->b)
		v6 = i1->b;
	this->b = v6;
	return *this;
}
