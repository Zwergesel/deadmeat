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
	int amount;
	bool active;
	ITEM_TYPE type;
	std::string strType;

public:
	static std::string ACTIVE_STRINGS[NUM_ITEM_TYPE];

	Item();
	Item(std::string name, uint format, symbol sym, TCODColor color, int amount);
	virtual ~Item();
	virtual Item* clone();

	ITEM_TYPE getType();
	symbol getSymbol();
	TCODColor getColor();
	std::string getName();
	uint getFormatFlags();
	virtual std::string toString();
	std::string typeString();
	int getAmount();
	void setAmount(int n);
	void changeAmount(int n);
	void setActive(bool set);
	bool isActive();
	bool canStackWith(Item* compare);

	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);
};

#endif