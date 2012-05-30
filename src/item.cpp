#include "item.hpp"

Item::Item(Point p, std::string n, int s, TCODColor c) : position(p), name(n), symbol(s), color(c)
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

Point Item::getPos()
{
	return position;
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