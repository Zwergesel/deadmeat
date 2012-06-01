#include "armor.hpp"
#include "savegame.hpp"

Armor::Armor() : Item("default", (unsigned char)'¤', TCODColor::pink), ac(0), hindrance(0), skill(SKILL_UNARMORED)
{
	type = ITEM_ARMOR;
	strType = "armor";
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

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Armor::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Armor", id);
	store ("name", name) ("symbol", symbol) ("color", color) ("ac", ac);
	store ("hindrance", hindrance) ("skill", (int)skill);
	sg << store;
	return id;
}

void Armor::load(LoadBlock& load)
{
	int s;
	load ("name", name) ("symbol", symbol) ("color", color) ("ac", ac);
	load ("hindrance", hindrance) ("skill", s);
	// TODO : check range 0 - SKILL_MAX(?)
	skill = static_cast<SKILLS>(s);
}