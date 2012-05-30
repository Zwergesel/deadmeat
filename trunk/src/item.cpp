#include "item.hpp"

Item::Item(Point p, std::string n, int s, TCODColor c) : position(p), name(n), symbol(s), color(c)
{
}

int Item::getSymbol()
{
  return symbol;
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