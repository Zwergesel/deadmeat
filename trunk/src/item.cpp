#include "item.hpp"
#include "savegame.hpp"

Item::Item()
{
	// constructor for savegames
	type = ITEM_DEFAULT;
	strType = "Item";
}

Item::Item(std::string n, uint f, symbol s, TCODColor c, bool stack):
	name(n), formatFlags(f), sym(s), color(c), stackable(stack)
{
	type = ITEM_DEFAULT;
	strType = "Item";
  count = 1;
}

Item::~Item() {}

Item* Item::clone()
{
	Item* copy = new Item(name, formatFlags, sym, color, stackable);
	copy->type = type;
	copy->strType = strType;
  copy->count = count;
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
	return getName();
}

std::string Item::typeString()
{
	return strType;
}

bool Item::isStackable()
{
  return stackable;
}

int Item::getCount()
{
  return count;
}

void Item::setCount(int c)
{
  count = c;
}

bool Item::stacksWith(Item* item)
{
  // TODO
  if(item->getType() == type && item->getName() == name) return true;
  return false;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Item::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Item", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color) ("stackable", stackable) ("count", count);
	sg << store;
	return id;
}

void Item::load(LoadBlock& load)
{
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color) ("stackable", stackable) ("count", count);
}