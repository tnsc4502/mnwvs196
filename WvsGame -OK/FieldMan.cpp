#include "FieldMan.h"
#include "Wz\WzResMan.hpp"
#include "Memory\MemoryPoolMan.hpp"

#include <mutex>

std::mutex fieldManMutex;

FieldMan::FieldMan()
{
}

FieldMan::~FieldMan()
{
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
	Field* newField = new Field();
	auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldID / 100000000)][std::to_string(nFieldID)];
	auto& infoData = mapWz["info"];

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
	newField->SetFirstUserEnter(infoData["onFirstUerEnter"]);
	newField->SetUserEnter(infoData["onUserEnter"]);

	//地圖長寬
	int mapSizeX = abs((int)infoData["VRRight"] - (int)infoData["VRLeft"]);
	int mapSizeY = abs((int)infoData["VRTop"] - (int)infoData["VRBottom"]);

	newField->SetMapSizeX(mapSizeX);
	newField->SetMapSizeY(mapSizeY);

	mField[nFieldID] = newField;
	mField[nFieldID]->SetFieldID(nFieldID);
	mField[nFieldID]->InitLifePool();
}

Field* FieldMan::GetField(int nFieldID)
{
	printf("Get Field ID = %d\n", nFieldID);
	//Prevent Double Registerations Or Enter On-Registering Map
	auto fieldResult = mField.find(nFieldID);
	if (fieldResult == mField.end())
		RegisterField(nFieldID);
	return mField[nFieldID];
}