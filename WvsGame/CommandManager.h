#pragma once
#include <string>

class User;

class CommandManager
{
private:
	CommandManager();
	~CommandManager();

public:
	static CommandManager* GetInstance();

	void Process(User* pUser, const std::string& input);
};

