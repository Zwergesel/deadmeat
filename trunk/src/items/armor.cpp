#include "armor.hpp"
#include "savegame.hpp"
#include "../world.hpp"

Armor::Armor()
{
	// empty constructor, for savegames
	type = ITEM_ARMOR;
	strType = "armor";
}

Armor::Armor(std::string n, uint f, symbol s, TCODColor c, int x, int w, int d, int e, int h, ArmorSlot as):
	Item(n,f,s,c,x,w), defense(d), enchantment(e), hindrance(h), slot(as)
{
	type = ITEM_ARMOR;
	strType = "armor";
}

Armor::~Armor() {}

Item* Armor::clone()
{
	Armor* copy = new Armor(name, formatFlags, sym, color, amount, weight, defense, enchantment, hindrance, slot);
	copy->active = active;
	return copy;
}

int Armor::getDefense()
{
	return defense;
}

int Armor::getEnchantment()
{
	return enchantment;
}

int Armor::getHindrance()
{
	return hindrance;
}

ArmorSlot Armor::getSlot()
{
	return slot;
}

std::string Armor::toString()
{
	std::stringstream ss;
	ss << (enchantment < 0 ? "" : "+") << enchantment << " " << name;
	return ss.str();
}

void Armor::randomize(int level)
{
	int roll = rng->getInt(0,999);
	enchantment = (roll / 800) + (roll / 950) + (roll / 995);
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Armor::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Armor", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	store ("color", color) ("amount", amount) ("weight", weight) ("active", active);
	store ("defense", defense) ("hindrance", hindrance) ("slot", (int)slot);
	store ("enchantment", enchantment);
	sg << store;
	return id;
}

void Armor::load(LoadBlock& load)
{
	int s;
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	load ("color", color) ("amount", amount) ("weight", weight) ("active", active);
	load ("defense", defense) ("hindrance", hindrance) ("slot", s);
	load ("enchantment", enchantment);
	if (s < 0 || s >= NUM_ARMOR_SLOTS) throw SavegameFormatException("Armor::load _ slot out of range");
	slot = static_cast<ArmorSlot>(s);
}