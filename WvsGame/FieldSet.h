#pragma once
#include <map>
#include <vector>
#include <string>

class User;
class Field;
class Script;
class AsyncScheduler;
class OutPacket;

class FieldSet
{
	std::map<std::string, std::string> m_mVariable;

	std::vector<int> m_aFieldID;
	std::vector<Field*> m_aField;

	std::string m_sFieldSetName, m_sScriptName;
	int m_nTimeLimit, m_nStartTime;
	Script* m_pScript;

	AsyncScheduler *m_pFieldSetTimer;

public:
	FieldSet();
	~FieldSet();

	void Init(const std::string& sCfgFilePath);
	const std::string& GetFieldSetName() const;
	void Enter(int nCharacterID, int nFieldInfo);
	int TryEnter(const std::vector<User*>& lpUser, int nFieldIdx, int nEnterChar);
	void Update();

	void OnUserEnterField(User* pUser);
	void MakeClockPacket(OutPacket& oPacket);
};

