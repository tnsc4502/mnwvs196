#include "ReactorTemplate.h"
#include "..\WvsLib\Wz\WzResMan.hpp"


ReactorTemplate::ReactorTemplate()
{
}

void ReactorTemplate::RegisterReactor(int nTemplateID, void * pImg)
{
	auto& ref = *((WZ::Node*)pImg);
	ReactorTemplate *pTemplate = new ReactorTemplate;

	m_mReactorTemplate[nTemplateID] = pTemplate;
}

void ReactorTemplate::Load()
{
	auto& ref = stWzResMan->GetWz(Wz::Reactor);
	for (auto& reactor : ref)
		RegisterReactor(atoi(reactor.Name().c_str()), &reactor);
}
