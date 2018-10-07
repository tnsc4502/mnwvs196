#pragma once
#include <string>

struct BridleItem
{
	std::string sItemName;
	int nItemID,
		nCreateItemID,
		dwTargetMobID,
		nBridleMsgType,
		nBridleProp,
		nBridleHP,
		nUseDelay;

	double dBridlePropChg;
};

