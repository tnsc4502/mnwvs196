#pragma once
#include <string>
#include <map>

class FieldSet;
class Field;

class FieldMan
{
private:
	std::map<int, Field*> m_mField;
	std::map<std::string, FieldSet*> m_mFieldSet;

	FieldMan();

public:

	static FieldMan *GetInstance();
	void RegisterField(int nFieldID);
	void FieldFactory(int nFieldID);
	void LoadFieldSet();
	Field* GetField(int nFieldID);
	FieldSet* GetFieldSet(const std::string& sFieldSetName);
	void RestoreFoothold(Field* pField, void *pPropFoothold, void *pLadderOrRope, void *pInfo);
	~FieldMan();
};

