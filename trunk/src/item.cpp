#include "item.hpp"

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