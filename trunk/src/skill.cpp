#include "skill.hpp"

Skill::Skill()
{
}

Skill::Skill(std::string name, int maxValue, ATTRIBUTE att)
{
	this->name = name;
	this->value = 0;
	this->maxValue = maxValue;
	this->att = att;
	this->exp = 0;
}

int Skill::expNeeded(int currentValue)
{
	return 250 * (currentValue+2) * (currentValue+1);
}