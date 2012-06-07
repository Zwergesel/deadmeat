#include "skill.hpp"

Skill::Skill()
{
}

Skill::Skill(std::string name, int value, ATTRIBUTE att)
{
	this->name = name;
	this->value = value;
	this->att = att;
	this->exp = 0;
	this->used = 0;
}

int Skill::expNeeded(int currentValue)
{
  return 250 * (currentValue+2) * (currentValue+1);
}