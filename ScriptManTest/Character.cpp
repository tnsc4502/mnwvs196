#include "Character.h"

Character::Character(const char* name, int hp) {
	this->name = name;
	health = hp;
}

void Character::say(const char* text) {
	std::cout << name << ":" << text << std::endl;
}

void Character::heal(Character* character) {
	character->setHealth(100);
}