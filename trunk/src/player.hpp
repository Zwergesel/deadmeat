#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <libtcod.hpp>
#include <string>
#include <vector>
#include <sstream>
#include "utility.hpp"
#include "skill.hpp"

class Creature;
class Level;
class Item;
class Weapon;
class Armor;
class Savegame;
class LoadBlock;

enum STATE
{
  STATE_DEFAULT,
  STATE_INVENTORY,
  STATE_PICKUP,
  STATE_WIELD,
  STATE_WEAR,
  STATE_DROP,
  STATE_DRESSING,
  NUM_STATE
};

typedef std::vector<std::pair<int, Item*> >::iterator InventoryIterator;

class Player
{
private:
	std::string name;
	Creature* creature;
	Skill skills[NUM_SKILL];
	int attributes[NUM_ATTR];
	std::vector<std::pair<int, Item*> > inventory;
	static int dx[9];
	static int dy[9];
	STATE state;
	int computeAttackBonus(Weapon* w);
	int computeArmorBonus(Armor* w);
	int actionMove(int direction);
	int actionLook(Point p);
	int actionPickup();
	int actionPickup(Item*);
	int actionWield(Item*);
	int actionWear(Item*);
	int actionDrop();
	int actionDrop(Item*);

public:
	Player();
	Player(std::string name);
	~Player();
	std::string getName();
	Creature* getCreature();
	int action();
	TCOD_key_t waitForKeypress(bool clBuf);
	bool addItem(Item* i);
	void removeItem(Item* i, bool del);
	std::vector<std::pair<int, Item*> > getInventory();
	Item* getInventoryItem(int item);
	STATE getState();
	int getAttribute(ATTRIBUTE attr);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif