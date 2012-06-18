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
  ITEM_DEFAULT,
  ITEM_WEAPON,
  ITEM_AMMO,
  ITEM_ARMOR,
  ITEM_FOOD,
  NUM_ITEM_TYPE
};

class Item
{
protected:
	std::string name;
	uint formatFlags;
	symbol sym;
	TCODColor color;
	ITEM_TYPE type;
	std::string strType;
  int count;
  bool stackable;

public:
	Item();
	Item(std::string name, uint format, symbol sym, TCODColor color, bool stackable = false);
	virtual ~Item();
	virtual Item* clone();

	ITEM_TYPE getType();
	symbol getSymbol();
	TCODColor getColor();
	std::string getName();
	uint getFormatFlags();
	virtual std::string toString();
	std::string typeString();  
  bool isStackable();
  int getCount();
  void setCount(int count);
  bool stacksWith(Item* item);

	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);
};

#endif