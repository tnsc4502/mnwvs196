#include "FieldMan.h"
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
	mField[nFieldID] = new Field();
	if (mField[nFieldID]->GetFieldID() != 0)
		return;
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