#include "armor.hpp"
#include "savegame.hpp"

Armor::Armor()
{
	// empty constructor, for savegames
	type = ITEM_ARMOR;
	strType = "armor";
}

Armor::Armor(std::string n, symbol s, TCODColor c, int a, int h, ArmorSlot as, SKILLS k):
	Item(n, s, c), ac(a), hindrance(h), slot(as), skill(k)
{
	type = ITEM_ARMOR;
	strType = "armor";
}

Armor::~Armor() {}

Item* Armor::clone()
{
	Armor* copy = new Armor(name, sym, color, ac, hindrance, slot, skill);
	return copy;
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
	store ("name", name) ("symbol", sym) ("color", color) ("ac", ac);
	store ("hindrance", hindrance) ("slot", (int)slot) ("skill", (int)skill);
	sg << store;
	return id;
}

void Armor::load(LoadBlock& load)
{
	int s1, s2;
	load ("name", name) ("symbol", sym) ("color", color) ("ac", ac);
	load ("hindrance", hindrance) ("slot", s1) ("skill", s2);
	if (s1 < 0 || s1 >= NUM_ARMOR_SLOTS) throw SavegameFormatException("Armor::load _ slot out of range");
	if (s2 < 0 || s2 >= NUM_SKILL) throw SavegameFormatException("Armor::load _ skill out of range");
	slot = static_cast<ArmorSlot>(s1);
	skill = static_cast<SKILLS>(s2);
}