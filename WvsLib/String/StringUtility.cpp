#include "StringUtility.h"

void StringUtility::Split(const std::string & str, std::vector<std::string>& result, const std::string & delimeter)
{
	int prevPos = 0, nextPos = 0, size = (int)str.size();
	while ((nextPos = (int)str.find(delimeter, prevPos)) >= 0 && nextPos < size)
	{
		result.push_back(std::move(str.substr(prevPos, nextPos - prevPos)));
		prevPos = nextPos + 1;
	}
	result.push_back(std::move(str.substr(prevPos, size - prevPos)));
}

void StringUtility::Split(const std::string & str, std::vector<int>& result, const std::string & delimeter)
{
	int prevPos = 0, nextPos = 0, size = (int)str.size();
	while ((nextPos = (int)str.find(delimeter, prevPos)) >= 0 && nextPos < size)
	{
		result.push_back(atoi(str.substr(prevPos, nextPos - prevPos).c_str()));
		prevPos = nextPos + 1;
	}
	result.push_back(atoi(str.substr(prevPos, size - prevPos).c_str()));
}

void StringUtility::Split(const std::string & str, std::vector<double>& result, const std::string & delimeter)
{
	int prevPos = 0, nextPos = 0, size = (int)str.size();
	while ((nextPos = (int)str.find(delimeter, prevPos)) >= 0 && nextPos < size)
	{
		result.push_back(atof(str.substr(prevPos, nextPos - prevPos).c_str()));
		prevPos = nextPos + 1;
	}
	result.push_back(atof(str.substr(prevPos, size - prevPos).c_str()));
}

std::string StringUtility::LeftPadding(std::string result, int totalSize, char paddingChar)
{
	while ((int)result.size() < totalSize)
		result = paddingChar + result;
	return result;
}

std::string StringUtility::RightPadding(std::string result, int totalSize, char paddingChar)
{
	while ((int)result.size() < totalSize)
		result += paddingChar;
	return result;
}
