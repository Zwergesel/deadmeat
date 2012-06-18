#include "item.hpp"
#include "savegame.hpp"
#include <cassert>

Item::Item()
{
	// constructor for savegames
	type = ITEM_DEFAULT;
	strType = "Item";
}

Item::Item(std::string n, uint f, symbol s, TCODColor c, int x):
	name(n), formatFlags(f), sym(s), color(c), amount(x)
{
	type = ITEM_DEFAULT;
	strType = "Item";
}

Item::~Item() {}

Item* Item::clone()
{
	Item* copy = new Item(name, formatFlags, sym, color, amount);
	copy->type = type;
	copy->strType = strType;
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

bool Item::canStackWith(Item* compare)
{
	if (compare->getType() != type) return false;
	if (type == ITEM_WEAPON || type == ITEM_ARMOR) return false;
	if (compare->getName() != name) return false;
	// TODO: subclass compare function call
	return true;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Item::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Item", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color) ("amount", amount);
	sg << store;
	return id;
}

void Item::load(LoadBlock& load)
{
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color) ("amount", amount);
}