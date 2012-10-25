#include "item.hpp"
#include "savegame.hpp"
#include <cassert>

extern std::map<std::string, std::vector<std::string>> Item::DestructionTable;

std::string Item::ACTIVE_STRINGS[NUM_ITEM_TYPE] = { "ERR_GOLD_ACTIVE", "wielded", "in quiver", "worn", "ERR_FOOD_ACTIVE", "ERR_CORPSE_ACTIVE", "active", "ERR_POTION_ACTIVE", "ERR_SPELLBOOK_ACTIVE", "ERR_DEFAULT_ACTIVE" };

Item::Item()
{
	// constructor for savegames
	type = ITEM_DEFAULT;
	strType = "other options";
}

Item::Item(Name n, symbol s, TCODColor c, int x, int w):
	name(n), sym(s), color(c), amount(x), active(false), weight(w)
{
	type = ITEM_DEFAULT;
	strType = "other options";
}

Item::~Item() {}

Item* Item::clone()
{
	Item* copy = new Item(name, sym, color, amount, weight);
	copy->active = active;
	return copy;
}

symbol Item::getSymbol()
{
	return sym;
}

ITEM_TYPE Item::getType()
{
	return type;
}

TCODColor Item::getColor()
{
	return color;
}

Name Item::getName()
{
	return name;
}

std::string Item::toString()
{
	return name.name;
}

std::string Item::typeString()
{
	return strType;
}

int Item::getAmount()
{
	return amount;
}

void Item::setAmount(int n)
{
	assert(n > 0);
	amount = n;
}

int Item::getWeight()
{
	return weight;
}

void Item::changeAmount(int n)
{
	assert(amount + n >= 0);
	amount += n;
}

void Item::setActive(bool set)
{
	active = set;
}

bool Item::isActive()
{
	return active;
}

bool Item::canStackWith(Item* compare)
{
	if (compare->getType() != type) return false;
	if (type == ITEM_WEAPON || type == ITEM_ARMOR || type == ITEM_CORPSE) return false;
	if (compare->getName().name != name.name) return false;
	// TODO: subclass compare function call (or override?)
	return true;
}

void Item::randomize(int level)
{
}

Item PSEUDOITEM_NOTHING = Item(Name("nothing"), '#', TCODColor::pink, 1, 0);

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Item::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Item", id);
	store ("name", name) ("symbol", sym) ("color", color);
	store ("amount", amount) ("weight", weight) ("active", active);
	sg << store;
	return id;
}

void Item::load(LoadBlock& load)
{
	load ("name", name) ("symbol", sym) ("color", color);
	load ("amount", amount) ("weight", weight) ("active", active);
}