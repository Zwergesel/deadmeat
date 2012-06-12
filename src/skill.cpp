#include "skill.hpp"

Skill::Skill()
{
}

Skill::Skill(std::string n, int v, int mv, int ml, int e):
	name(n),value(v),maxValue(mv),maxLevel(ml),exp(e)
{
	req.resize(maxLevel);
}

int Skill::expNeeded(int currentValue)
{
	return 250 * (currentValue+2) * (currentValue+1);
}