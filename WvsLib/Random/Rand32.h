#pragma once
#include <mutex>
#include <vector>

class Rand32
{
	std::mutex m_lock;
	unsigned int m_s1, m_s2, m_s3, m_past_s1, m_past_s2, m_past_s3;

	Rand32();
	void Seed(unsigned int s1, unsigned int s2, unsigned int s3);

public:
	static Rand32* GetInstance();
	unsigned int Random();
	unsigned int Random(unsigned int nMin, unsigned int nMax);
	std::vector<int> GetRandomUniqueArray(int nStart, int nRange, int nCount);
};