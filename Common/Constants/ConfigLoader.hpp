#pragma once
#include <fstream>
#include <string>
#include <map>

class ConfigLoader
{
public:
	static const char cfgDelimiter = '=';

	struct VALUE_TYPE 
	{
		char type;
		VALUE_TYPE() {}
		~VALUE_TYPE() {}
		//union
		//{
			std::string sValue;
			int iValue;
			double dValue;
		//};
	};

private:
	std::map<std::string, VALUE_TYPE> aSettings;

	ConfigLoader() {}

	void ParseConfig(const std::string& cfgFileName)
	{
		std::fstream cfgFile(cfgFileName, std::ios::in);
		if (!cfgFile)
		{
			printf("Unable to open config file \"%s\"\n", cfgFileName.c_str());
			return;
		}
		std::string line;
		int delimiterPos = 0, leftPos = 0, rightPos = 0;
		while(std::getline(cfgFile, line))
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
			char typeDelimiter = line[rightPos++];
			//printf("Type Delimiter = %c  str value = %s\n", typeDelimiter, line.substr(rightPos, line.size() - rightPos + 1).c_str());
			auto keyStr = line.substr(0, leftPos);
			switch (typeDelimiter)
			{
			case 's':
				aSettings[keyStr].sValue = line.substr(rightPos, line.size() - rightPos + 1);
				break;
			case 'd':
				aSettings[keyStr].dValue = atof(line.substr(rightPos, line.size() - rightPos + 1).c_str());
				break;
			case 'i':
				aSettings[keyStr].iValue = atoi(line.substr(rightPos, line.size() - rightPos + 1).c_str());
				break;
			}
			aSettings[keyStr].type = typeDelimiter;
		}
	}

public:
	static ConfigLoader* GetInstance()
	{
		static ConfigLoader *pLoader = new ConfigLoader();
		return pLoader;
	}
	
	void LoadConfig(const std::string& cfgFileName)
	{
		ParseConfig(cfgFileName);
	}

	std::string StrValue(const std::string &key)
	{
		auto findResult = aSettings.find(key);
		if (findResult == aSettings.end())
			return 0;
		return findResult->second.sValue;
	}

	int IntValue(const std::string &key)
	{
		auto findResult = aSettings.find(key);
		if (findResult == aSettings.end())
			return 0;
		return findResult->second.iValue;
	}

	double DoubleValue(const std::string &key)
	{
		auto findResult = aSettings.find(key);
		if (findResult == aSettings.end())
			return 0;
		return findResult->second.dValue;
	}
};