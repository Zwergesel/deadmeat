#include "armor.hpp"
#include "savegame.hpp"

Armor::Armor() : Item("default", (unsigned char)'¤', TCODColor::pink), ac(0), hindrance(0), slot(ARMOR_BODY), skill(SKILL_UNARMORED)
{
	type = ITEM_ARMOR;
	strType = "armor";
}

Armor::Armor(std::string n, int sym, TCODColor c, int a, int h, ArmorSlot as, SKILLS s) : Item(n, sym, c), ac(a), hindrance(h), slot(as), skill(s)
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

ArmorSlot Armor::getSlot()
{
	return slot;
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
	store ("hindrance", hindrance) ("slot", (int)slot) ("skill", (int)skill);
	sg << store;
	return id;
}

void Armor::load(LoadBlock& load)
{
	int s1, s2;
	load ("name", name) ("symbol", symbol) ("color", color) ("ac", ac);
	load ("hindrance", hindrance) ("slot", s1) ("skill", s2);
	if (s1 < 0 || s1 >= NUM_ARMOR_SLOTS) throw SavegameFormatException("Armor::load _ slot out of range");
	if (s2 < 0 || s2 >= NUM_SKILL) throw SavegameFormatException("Armor::load _ skill out of range");
	slot = static_cast<ArmorSlot>(s1);
	skill = static_cast<SKILLS>(s2);
}