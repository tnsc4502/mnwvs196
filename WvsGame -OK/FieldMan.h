#pragma once
#include <map>
#include "Field.h"

class FieldMan
{
private:
	std::map<int, Field*> mField;
	FieldMan();

public:

	static FieldMan *GetInstance()
	{
		static FieldMan *sPtrFieldMan = new FieldMan();
		return sPtrFieldMan;
	}

	void RegisterField(int nFieldID);
	void FieldFactory(int nFieldID);
	Field* GetField(int nFieldID);

	~FieldMan();
};

