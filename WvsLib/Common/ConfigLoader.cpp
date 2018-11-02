#include "ConfigLoader.hpp"

void ConfigLoader::ParseConfig(const std::string & cfgFileName)
{
	aSettings.clear();
	std::fstream cfgFile(cfgFileName, std::ios::in);
	if (!cfgFile)
	{
		printf("Unable to open config file \"%s\"\n", cfgFileName.c_str());
		return;
	}
	std::string line;
	int delimiterPos = 0, leftPos = 0, rightPos = 0;
	while (std::getline(cfgFile, line))
	{
		delimiterPos = (int)line.find(cfgDelimiter, 0);
		if (delimiterPos < 0 || delimiterPos > line.size())
		{
			printf("Error while parsing config file, please check that each line has the format : <KEY>,<VALUE>");
			return;
		}
		leftPos = rightPos = delimiterPos;
		++rightPos;
		while (leftPos - 1 >= 0 && line[rightPos - 1] == ' ')--leftPos;
		while (rightPos + 1 < line.size() && line[rightPos + 1] == ' ')++rightPos;
		auto keyStr = line.substr(0, leftPos);
		aSettings[keyStr] = line.substr(rightPos, line.size() - rightPos + 1);
	}
}

ConfigLoader * ConfigLoader::GetInstance()
{
	static ConfigLoader *pLoader = new ConfigLoader();
	return pLoader;
}

void ConfigLoader::LoadConfig(const std::string & cfgFileName)
{
	ParseConfig(cfgFileName);
}

std::string ConfigLoader::StrValue(const std::string & key)
{
	auto findResult = aSettings.find(key);
	if (findResult == aSettings.end())
		return "";
	return findResult->second;
}

int ConfigLoader::IntValue(const std::string & key)
{
	auto findResult = aSettings.find(key);
	if (findResult == aSettings.end())
		return 0;
	return atoi(findResult->second.c_str());
}

double ConfigLoader::DoubleValue(const std::string & key)
{
	auto findResult = aSettings.find(key);
	if (findResult == aSettings.end())
		return 0;
	return atof(findResult->second.c_str());
}