#pragma once
#include <iostream>

class Character {
public:
	Character(const char* name, int hp);
	void say(const char* text);
	void heal(Character* character);
	const char* getName() { return name; }
	int getHealth() { return health; }
	void setHealth(int hp) { health = hp; }
private:
	const char* name;
	int health;
};

