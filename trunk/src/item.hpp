#ifndef _ITEM_HPP
#define _ITEM_HPP

#include <libtcod.hpp>
#include "utility.hpp"
#include <string>

class Item
{
private:
	int symbol;
	TCODColor color;
	Point position;
	std::string name;

public:
	Item(Point position, std::string name, int symbol, TCODColor color);
	int getSymbol();
	TCODColor getColor();
	Point getPos();
	std::string getName();
};

#endif