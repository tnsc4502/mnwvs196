#include "TemporaryStat.h"
#include "..\WvsLib\Net\OutPacket.h"

TemporaryStat::TS_Flag::TS_Flag(int dwFlagValue)
{
	m_nFlagPos = dwFlagValue / 32;
	int nValue = (1 << (31 - (dwFlagValue % 32)));
	for (int i = 0; i < FLAG_COUNT; ++i)
		m_aData[i] = 0;
	m_aData[m_nFlagPos] |= nValue;
}

void TemporaryStat::TS_Flag::Encode(OutPacket * oPacket)
{
	for (int i = 0; i < FLAG_COUNT; ++i) {
		//printf("Encode TS Flag : [%d] = %d\n", i, m_aData[i]);
		oPacket->Encode4(m_aData[i]);
	}
}

TemporaryStat::TS_Flag & TemporaryStat::TS_Flag::operator|=(const TS_Flag & rhs)
{
	m_aData[rhs.m_nFlagPos] |= rhs.m_aData[rhs.m_nFlagPos];
	return *this;
}

bool TemporaryStat::TS_Flag::operator&(const TS_Flag & rhs)
{
	return (m_aData[rhs.m_nFlagPos] & rhs.m_aData[rhs.m_nFlagPos]) != 0;
}

TemporaryStat::TS_Flag TemporaryStat::TS_Flag::GetDefault()
{
	TS_Flag ret(0);
	ret.m_aData[0] = 0;
	return ret;
}
