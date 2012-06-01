#include "armor.hpp"

Armor::Armor() : Item("default", '§', TCODColor::pink), ac(0), hindrance(0), skill(SKILL_UNARMORED)
{
}

Armor::Armor(std::string n, int sym, TCODColor c, int a, int h, SKILLS s) : Item(n, sym, c), ac(a), hindrance(h), skill(s)
{
	type = ITEM_ARMOR;
	strType = "armor";
}

int Armor::getAC()
{
	return ac;
}

int Armor::getHindrance()
{
	return hindrance;
}

SKILLS Armor::getSkill()
{
	return skill;
}
