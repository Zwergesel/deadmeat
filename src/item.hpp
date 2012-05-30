#ifndef _ITEM_HPP
#define _ITEM_HPP

#include <libtcod.hpp>
#include "utility.hpp"
#include <string>

enum ITEM_TYPE
{
  ITEM_WEAPON,
  ITEM_DEFAULT
};

class Item
{
private:
	int symbol;
	TCODColor color;
	Point position;
	std::string name;

protected:
	ITEM_TYPE type;

public:
	Item(Point position, std::string name, int symbol, TCODColor color);
	ITEM_TYPE getType();
	int getSymbol();
	TCODColor getColor();
	Point getPos();
	std::string getName();
};

#endif