#include "FieldMan.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "TimerThread.h"

#include "Field.h"
#include "FieldSet.h"
#include "PortalMap.h"
#include "ReactorPool.h"

#include <mutex>
#include <filesystem>
#include <fstream>
#include <streambuf>

std::mutex fieldManMutex;

namespace fs = std::experimental::filesystem;

FieldMan::FieldMan()
{
}

FieldMan::~FieldMan()
{
}

FieldMan * FieldMan::GetInstance()
{
	static FieldMan *sPtrFieldMan = new FieldMan();
	return sPtrFieldMan;
}

void FieldMan::RegisterField(int nFieldID)
{
	std::lock_guard<std::mutex> guard(fieldManMutex);
	FieldFactory(nFieldID);
}

void FieldMan::FieldFactory(int nFieldID) 
{
	/*if (mField[nFieldID]->GetFieldID() != 0)
		return;*/
	/*
	在這裡檢查此Field的型態，舊版的在這裡根據FieldType建立不同的地圖實體
	Field
	Field_Tutorial
	Field_ShowaBath
	Field_WeddingPhoto
	Field_SnowBall
	Field_Tournament
	Field_Coconut
	Field_OXQuiz
	Field_PersonalTimeLimit
	Field_GuildBoss
	Field_MonsterCarnival
	Field_Wedding
	...
	*/
	std::string fieldStr = std::to_string(nFieldID);
	while (fieldStr.size() < 9)
		fieldStr = "0" + fieldStr;
	Field* newField = new Field();
	auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldID / 100000000)][fieldStr];
	mapWz = stWzResMan->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldID / 100000000)][std::to_string(nFieldID)];
	auto& infoData = mapWz["info"];

	//if (infoData) {
		newField->SetCould(((int)infoData["cloud"] != 0));
		newField->SetTown(((int)infoData["town"] != 0));
		newField->SetSwim(((int)infoData['swim'] != 0));
		newField->SetFly(((int)infoData['fly'] != 0));
		newField->SetReturnMap(infoData["returnMap"]);
		newField->SetForcedReturn(infoData["forcedReturn"]);
		newField->SetMobRate(infoData["mobRate"]);
		newField->SetFieldType(infoData["fieldType"]);
		newField->SetFieldLimit(infoData["fieldLimit"]);
		newField->SetCreateMobInterval(infoData["createMobInterval"]);
		newField->SetFiexdMobCapacity(infoData["fixedMobCapacity"]);

		//地圖長寬
		int mapSizeX = abs((int)infoData["VRRight"] - (int)infoData["VRLeft"]);
		int mapSizeY = abs((int)infoData["VRTop"] - (int)infoData["VRBottom"]);

		newField->SetFirstUserEnter(infoData["onFirstUerEnter"]);
		newField->SetUserEnter(infoData["onUserEnter"]);

		//以下資訊不可靠，有些地圖(沒有該屬性)無法算
		newField->SetMapSizeX(mapSizeX);
		newField->SetMapSizeY(mapSizeY);
		//WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "New Field Size X = %d, Y = %d\n", (int)infoData["forcedReturn"], mapSizeY);
	//}
	newField->GetPortalMap()->RestorePortal(newField, &(mapWz["portal"]));
	newField->GetReactorPool()->Init(newField, &(mapWz["reactor"]));
	newField->SetFieldID(nFieldID);
	newField->InitLifePool();

	m_mField[nFieldID] = newField;
	TimerThread::RegisterField(newField);
}

void FieldMan::LoadFieldSet()
{
	std::string strPath = "./DataSrv/FieldSet";
	for (auto &file : fs::directory_iterator(strPath))
	{
		FieldSet *pFieldSet = new FieldSet;
		std::wstring wStr = file.path();
		//Convert std::wstring to std::string, note that the path shouldn't include any NON-ASCII character.
		pFieldSet->Init(std::string{ wStr.begin(), wStr.end() });
		m_mFieldSet[pFieldSet->GetFieldSetName()] = pFieldSet;

		std::cout << file << std::endl; 
		std::ifstream t(file.path());
		std::string str((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		std::cout << str << std::endl;
	}
}

Field* FieldMan::GetField(int nFieldID)
{
	//printf("Get Field ID = %d\n", nFieldID);
	//Prevent Double Registerations Or Enter On-Registering Map
	auto fieldResult = m_mField.find(nFieldID);
	if (fieldResult == m_mField.end())
		RegisterField(nFieldID);
	return m_mField[nFieldID];
}

FieldSet * FieldMan::GetFieldSet(const std::string & sFieldSetName)
{
	auto fieldResult = m_mFieldSet.find(sFieldSetName);
	if (fieldResult == m_mFieldSet.end())
		return nullptr;
	return fieldResult->second;
}
