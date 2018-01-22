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

	static std::string LeftPadding(std::string result, int totalSize, char paddingChar)
	{
		while ((int)result.size() < totalSize)
			result = paddingChar + result;
		return result;
	}

	static std::string RightPadding(std::string result, int totalSize, char paddingChar)
	{
		while ((int)result.size() < totalSize)
			result += paddingChar;
		return result;
	}

	template<typename T>
	static std::string VectorToString(const std::vector<T>& container, const std::string& delimeter)
	{
		std::string ret = "";
		int nSize = (int)container.size(), i = 0;
		for(auto iter = container.begin(); iter != container.end(); ++iter)
			ret += (std::to_string(*iter) + (i++ != nSize - 1 ? delimeter : ""));
		//for (int i = 0; i < nSize; ++i)
		//	ret += container[i] + (i != nSize - 1 ? delimeter : "");
		return ret;
	}
};