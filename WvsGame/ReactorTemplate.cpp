#include "ReactorTemplate.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

std::map<int, ReactorTemplate*> ReactorTemplate::m_mReactorTemplate;

ReactorTemplate::ReactorTemplate()
{
}

void ReactorTemplate::RegisterReactor(int nTemplateID, void * pImg)
{
	auto& ref = *((WZ::Node*)pImg);
	auto empty = WZ::Node();
	ReactorTemplate *pTemplate = AllocObj(ReactorTemplate);
	pTemplate->m_nTemplateID = nTemplateID;
	for (int i = 0; ; ++i)
	{
		pTemplate->m_aStateInfo.push_back(StateInfo{});
		auto* pStateInfo = &(pTemplate->m_aStateInfo[pTemplate->m_aStateInfo.size() - 1]);
		auto& node = ref[std::to_string(i)];
		if (node == empty)
			break;
		LoadEvent(pStateInfo, &(node));
	}
	pTemplate->m_nReqHitCount = ref["info"]["hitCount"];
	pTemplate->m_tHitDelay = ref["info"]["delay"];
	pTemplate->m_bRemoveInFieldSet = ((int)ref["info"]["removeInFieldSet"] == 1 ? true : false);
	m_mReactorTemplate[nTemplateID] = pTemplate;
}

void ReactorTemplate::LoadEvent(StateInfo * pInfo, void * pImg)
{
	auto& ref_ = (*((WZ::Node*)pImg));
	auto& ref = ref_["event"];
	auto empty = WZ::Node();
	if (ref == empty)
		return;
	EventInfo eventInfo;

	for (int i = 0; ; ++i)
	{
		auto& node = ref[std::to_string(i)];
		if (node == empty)
			break;

		eventInfo.m_nType = (int)node["type"];
		eventInfo.m_nStateToBe = (int)node["state"];
		eventInfo.m_tHitDelay = (int)node["delay"];
		
		auto& lt = node["lt"];
		if (lt != empty)
		{
			auto& rb = node["rb"];
			eventInfo.m_aSpaceVertex[0].x = (int)lt["x"];
			eventInfo.m_aSpaceVertex[0].y = (int)lt["y"];
			eventInfo.m_aSpaceVertex[1].x = (int)rb["x"];
			eventInfo.m_aSpaceVertex[1].y = (int)rb["y"];
			eventInfo.m_bCheckTargetRange = true;
		}
		if (ref_["info"]["activateByTouch"] != empty)
			eventInfo.m_nTouchable = 2;
		else
			eventInfo.m_nTouchable =
				(node["2"] != empty && (int)node["2"] > 0) ||
				(node["clickArea"] != empty) ||
				(eventInfo.m_nType == 9);

		eventInfo.m_nItemID = (int)node["0"];
		eventInfo.m_nCount = (int)node["1"];


		//printf("event : %d, type = %d, state = %d,%s\n", i, eventInfo.m_nType, eventInfo.m_nStateToBe, ((std::string)node["lt"]["x"]).c_str());
		pInfo->m_aEventInfo.push_back(eventInfo);
		pInfo->m_tTimeout = (int)ref["timeOut"];
		pInfo->m_tHitDelay = (int)ref["delay"];
	}
}

void ReactorTemplate::Load()
{
	auto& ref = stWzResMan->GetWz(Wz::Reactor);
	for (auto& reactor : ref)
		RegisterReactor(atoi(reactor.Name().c_str()), &reactor);
}

ReactorTemplate * ReactorTemplate::GetReactorTemplate(int nTemplateID)
{
	auto findIter = m_mReactorTemplate.find(nTemplateID);
	if (findIter == m_mReactorTemplate.end())
		return nullptr;
	return findIter->second;
}

ReactorTemplate::StateInfo * ReactorTemplate::GetStateInfo(int nState)
{
	if (nState < 0 || nState >= m_aStateInfo.size())
		return nullptr;
	return &(m_aStateInfo[nState]);
}

ReactorTemplate::EventInfo * ReactorTemplate::GetEventInfo(int nState, int nEventIdx)
{
	auto pInfo = GetStateInfo(nState);
	if (pInfo == nullptr || (nEventIdx < 0) || (nEventIdx >= pInfo->m_aEventInfo.size()))
		return nullptr;
	return &(pInfo->m_aEventInfo[nEventIdx]);
}
