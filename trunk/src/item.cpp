#include "item.hpp"
#include "savegame.hpp"

Item::Item()
{
	// constructor for savegames
	type = ITEM_DEFAULT;
	strType = "Item";
}

Item::Item(std::string n, symbol s, TCODColor c):
	name(n), sym(s), color(c)
{
	type = ITEM_DEFAULT;
	strType = "Item";
}

Item::~Item() {}

Item* Item::clone()
{
	Item* copy = new Item(name, sym, color);
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

std::string Item::toString()
{
	return getName();
}

std::string Item::typeString()
{
	return strType;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Item::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Item", id);
	store ("name", name) ("symbol", sym) ("color", color);
	sg << store;
	return id;
}

void Item::load(LoadBlock& load)
{
	load ("name", name) ("symbol", sym) ("color", color);
}