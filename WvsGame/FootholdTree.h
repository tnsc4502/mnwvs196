#pragma once
#include <vector>
#include <algorithm>
#include "FieldPoint.h"

class StaticFoothold;

//TRSTree_InsertOnly<StaticFoothold> in official server.
//I don't want to make this a template class since this is used by WvsPhysicalSpace2D and contains StaticFoothold only.
class FootholdTree
{
public:
	struct I2
	{
		int l = 0, t = 0, r = 0, b = 0; //left, top, right, bottom

		bool Overlap(I2 *i2);
		I2& Union(I2 &i2);
	};

	struct NODE
	{
		const static int MAX_ENTRY = 5;
		NODE* pParent = nullptr;

		struct ENTRY
		{
			StaticFoothold *pS = nullptr;
			FieldPoint pt1, pt2;
			I2 i;
			NODE *pChild = nullptr;
		};

		ENTRY E[MAX_ENTRY];
		I2 iMBR;
		bool bValidMBR = false;
		int nLevel = 0, nCount = 0;

		void InsertEntry(ENTRY *e);
		void RecalcMBR();
		void RemoveAllEntries();
		void DeleteEntry(int nIdx);
		ENTRY* FindNodeEntry(NODE *pNode);
	};

private:
	std::vector<NODE*> m_aNode;
	NODE* m_pRoot = nullptr;
	void raw_AdjustTree(NODE *pNode, I2 *pi);
	NODE* raw_ChooseSubTree(I2 *i, int nLevel);
	void raw_Insert(NODE::ENTRY* e, std::vector<int>& lLev, int nLevel);
	void raw_Reinsert(NODE* pNode, std::vector<int>& lLev);
	NODE* raw_SplitNode(NODE* pNode);
	NODE* raw_OverflowTreatment(NODE *pNode, std::vector<int>& lLev);
	void raw_Search(NODE *pNode, I2 *i, std::vector<StaticFoothold*>& lRes);

public:
	FootholdTree();
	~FootholdTree();

	void InsertData(const FieldPoint& pt0, const FieldPoint& pt1, StaticFoothold* pData);
	std::vector<StaticFoothold*> Search(const FieldPoint& pt0, const FieldPoint& pt1);
};

