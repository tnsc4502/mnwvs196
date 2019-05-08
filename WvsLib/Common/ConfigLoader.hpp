#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "..\String\StringUtility.h"

class ConfigLoader
{
private:
	static const char cfgDelimiter = '=';
	std::map<std::string, std::string> aSettings;

	ConfigLoader() {};
	void ParseConfig(const std::string& cfgFileName);
	//static ConfigLoader* GetInstance();

public:
	static ConfigLoader* Get(const std::string& cfgFileName);

	void LoadConfig(const std::string& cfgFileName);
	std::string StrValue(const std::string &key);
	int IntValue(const std::string &key);
	double DoubleValue(const std::string &key);

	template<typename T>
	inline std::vector<T> GetArray(const std::string & key)
	{
		std::vector<T> ret;
		auto findResult = aSettings.find(key);
		if (findResult != aSettings.end())
			StringUtility::Split(findResult->second, ret, ",");
		return ret;
	}
};
