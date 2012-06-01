#ifndef _ITEM_HPP
#define _ITEM_HPP

#include <libtcod.hpp>
#include "utility.hpp"
#include <string>
#include <sstream>

class Savegame;
class LoadBlock;

enum ITEM_TYPE
{
  ITEM_WEAPON,
  ITEM_ARMOR,
  ITEM_DEFAULT,
  NUM_ITEM_TYPE
};

class Item
{
protected:
	int symbol;
	TCODColor color;
	std::string name;
	ITEM_TYPE type;
	std::string strType;

public:
	Item(); // for savegames
	Item(std::string name, int symbol, TCODColor color);
	ITEM_TYPE getType();
	int getSymbol();
	TCODColor getColor();
	std::string getName();
	std::string toString();
	std::string typeString();

	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);
};

#endif