#pragma once
#include <map>
#include "Field.h"

class FieldMan
{
private:
	std::map<int, Field*> mField;
	FieldMan();

public:

	static FieldMan *GetInstance();
	void RegisterField(int nFieldID);
	void FieldFactory(int nFieldID);
	Field* GetField(int nFieldID);

	~FieldMan();
};

