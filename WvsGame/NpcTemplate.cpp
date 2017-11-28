#include "NpcTemplate.h"
#include "Memory\MemoryPoolMan.hpp"


WZ::Node* NpcTemplate::m_NpcWzProperty = &(stWzResMan->GetWz(Wz::Npc));

NpcTemplate::NpcTemplate()
{
}


NpcTemplate::~NpcTemplate()
{
}

NpcTemplate* NpcTemplate::GetNpcTemplate(int dwTemplateID)
{
}