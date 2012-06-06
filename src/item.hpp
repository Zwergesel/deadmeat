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
	std::string name;
	symbol sym;
	TCODColor color;
	ITEM_TYPE type;
	std::string strType;

public:
	Item();
	Item(std::string name, symbol sym, TCODColor color);
	virtual ~Item();
	virtual Item* clone();
	
	ITEM_TYPE getType();
	symbol getSymbol();
	TCODColor getColor();
	std::string getName();
	std::string toString();
	std::string typeString();

	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);
};

#endif