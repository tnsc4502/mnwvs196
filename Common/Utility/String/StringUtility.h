#pragma once

#include <vector>
#include <string>

class StringUtility
{
public:
	static void Split(const std::string & str, std::vector<std::string>& result, const std::string & delimeter)
	{
		int prevPos = 0, nextPos = 0, size = (int)str.size();
		while ((nextPos = (int)str.find(delimeter, prevPos)) >= 0 && nextPos < size)
		{
			result.push_back(std::move(str.substr(prevPos, nextPos - prevPos)));
			prevPos = nextPos + 1;
		}
		result.push_back(std::move(str.substr(prevPos, size - prevPos)));
	}
};