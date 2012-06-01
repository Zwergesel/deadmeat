#include "item.hpp"
#include "savegame.hpp"

Item::Item()
{
	// for savegames
	type = ITEM_DEFAULT;
	strType = "Item";
}

Item::Item(std::string n, int s, TCODColor c):
	symbol(s), color(c), name(n)
{
	type = ITEM_DEFAULT;
	strType = "Item";
}

int Item::getSymbol()
{
	return symbol;
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
	store ("name", name) ("symbol", symbol) ("color", color);
	sg << store;
	return id;
}

void Item::load(Savegame* sg, std::stringstream& ss)
{
	name = sg->loadString("name", ss);
	symbol = sg->loadInt("symbol", ss);
	color = sg->loadColor("color", ss);
}