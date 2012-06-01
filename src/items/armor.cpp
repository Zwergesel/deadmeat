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

unsigned int Armor::save(Savegame* sg)
{
	void* index = static_cast<void*>(this);
	if (sg->objExists(index)) return sg->objId(index);
	std::stringstream ss;
	sg->saveObj(index, "Armor", ss);
	sg->saveString(name, "name", ss);
	sg->saveInt(symbol, "symbol", ss);
	sg->saveColor(color, "color", ss);
	sg->saveInt(ac, "ac", ss);
	sg->saveInt(hindrance, "hindrance", ss);
	sg->saveInt(skill, "skill", ss);
	// Note: auto-load type and strType
	sg->flushStringstream(ss);
	return sg->objId(index);
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