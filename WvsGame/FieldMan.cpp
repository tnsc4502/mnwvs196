#include "FieldMan.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "TimerThread.h"

#include "Field.h"
#include "FieldSet.h"
#include "PortalMap.h"
#include "ReactorPool.h"
#include "WvsPhysicalSpace2D.h"

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
	auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldID / 100000000)][fieldStr];
	if (mapWz == WZ::Node())
		return;

	auto& infoData = mapWz["info"];
	Field* newField = AllocObj(Field);
	newField->SetFieldID(nFieldID);

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

	newField->GetPortalMap()->RestorePortal(newField, &(mapWz["portal"]));
	newField->GetReactorPool()->Init(newField, &(mapWz["reactor"]));
	RestoreFoothold(newField, &(mapWz["foothold"]), nullptr, &infoData);
	newField->InitLifePool();

	m_mField[nFieldID] = newField;
	TimerThread::RegisterField(newField);
}

void FieldMan::LoadFieldSet()
{
	std::string strPath = "./DataSrv/FieldSet";
	for (auto &file : fs::directory_iterator(strPath))
	{
		FieldSet *pFieldSet = AllocObj( FieldSet );
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

void FieldMan::RestoreFoothold(Field * pField, void * pPropFoothold, void * pLadderOrRope, void * pInfo)
{
	auto& refInfo = *((WZ::Node*)pInfo);
	int nFieldLink = (nFieldLink = atoi(((std::string)refInfo["link"]).c_str()));
	if ((nFieldLink != 0))
	{
		auto fieldStr = StringUtility::LeftPadding(std::to_string(nFieldLink), 9, '0');
		auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldLink / 100000000)][fieldStr];
		pInfo = &(mapWz["info"]);
		pPropFoothold = &(mapWz["foothold"]);
	}
	pField->GetSpace2D()->Load(pPropFoothold, pLadderOrRope, pInfo);
	pField->SetMapSize(
		pField->GetSpace2D()->GetRect().left,
		pField->GetSpace2D()->GetRect().top
	);
	pField->SetLeftTop(
		pField->GetSpace2D()->GetRect().right - pField->GetSpace2D()->GetRect().left,
		pField->GetSpace2D()->GetRect().bottom - pField->GetSpace2D()->GetRect().top
	);
}
