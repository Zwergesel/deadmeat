#include "armor.hpp"
#include "savegame.hpp"

Armor::Armor() : Item("default", (unsigned char)'�', TCODColor::pink), ac(0), hindrance(0), skill(SKILL_UNARMORED)
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

void Armor::load(Savegame* sg, std::stringstream& ss)
{
	name = sg->loadString("name", ss);
	symbol = sg->loadInt("symbol", ss);
	color = sg->loadColor("color", ss);
	ac = sg->loadInt("ac", ss);
	hindrance = sg->loadInt("hindrance", ss);
	// TODO : check range 0 - SKILL_MAX(?)
	skill = static_cast<SKILLS>(sg->loadInt("skill", ss));
}