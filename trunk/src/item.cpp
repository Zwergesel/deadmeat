#include "item.hpp"
#include "savegame.hpp"
#include <cassert>

std::string Item::ACTIVE_STRINGS[NUM_ITEM_TYPE] = { "wielded", "in quiver", "worn", "ERR_FOOD_ACTIVE", "active" };

Item::Item()
{
	// constructor for savegames
	type = ITEM_DEFAULT;
	strType = "other options";
}

Item::Item(std::string n, uint f, symbol s, TCODColor c, int x):
	name(n), formatFlags(f), sym(s), color(c), amount(x), active(false)
{
	type = ITEM_DEFAULT;
	strType = "other options";
}

Item::~Item() {}

Item* Item::clone()
{
	Item* copy = new Item(name, formatFlags, sym, color, amount);
	copy->type = type;
	copy->strType = strType;
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

std::string Item::getName()
{
	return name;
}

uint Item::getFormatFlags()
{
	return formatFlags;
}

std::string Item::toString()
{
	return name;
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
	if (type == ITEM_WEAPON || type == ITEM_ARMOR) return false;
	if (compare->getName() != name) return false;
	// TODO: subclass compare function call
	return true;
}

Item PSEUDOITEM_NOTHING = Item("nothing", F_PROPER, '#', TCODColor::pink, 1);

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Item::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Item", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	store ("color", color) ("amount", amount) ("active", active);
	sg << store;
	return id;
}

void Item::load(LoadBlock& load)
{
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	load ("color", color) ("amount", amount) ("active", active);
}