#pragma once

#include <vector>
#include <string>

class StringUtility
{
public:
	static void Split(const std::string & str, std::vector<std::string>& result, const std::string & delimeter);
	static void Split(const std::string & str, std::vector<int>& result, const std::string & delimeter);
	static void Split(const std::string & str, std::vector<double>& result, const std::string & delimeter);

	static std::string LeftPadding(std::string result, int totalSize, char paddingChar);
	static std::string RightPadding(std::string result, int totalSize, char paddingChar);

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