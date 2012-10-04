#ifndef _ITEM_HPP
#define _ITEM_HPP

#include <libtcod.hpp>
#include "utility.hpp"
#include <string>
#include <sstream>
#include <map>

class Savegame;
class LoadBlock;

enum ITEM_TYPE
{
  ITEM_GOLD,
  ITEM_WEAPON,
  ITEM_AMMO,
  ITEM_ARMOR,
  ITEM_FOOD,
  ITEM_CORPSE,
  ITEM_TOOL,
  ITEM_POTION,
  ITEM_SPELLBOOK,
  ITEM_DEFAULT,
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
	int weight;

public:
	static std::string ACTIVE_STRINGS[NUM_ITEM_TYPE];
	static std::map<std::string, std::vector<std::string>> DestructionTable;

	Item();
	Item(std::string name, uint format, symbol sym, TCODColor color, int amount, int weight);
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
	int getWeight();
	void setAmount(int n);
	void changeAmount(int n);
	void setActive(bool set);
	bool isActive();
	bool canStackWith(Item* compare);
	virtual void randomize(int level);

	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);
};

extern Item PSEUDOITEM_NOTHING;

#endif