#pragma once
#include "User.h"

class Controller
{
	User *pUser;
public:
	Controller(User *ptrUser);
	~Controller();
};

