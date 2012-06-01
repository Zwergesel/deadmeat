#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <libtcod.hpp>
#include <string>
#include <vector>
#include "utility.hpp"
#include "skill.hpp"

class Creature;
class Level;
class Item;
class Weapon;
class Armor;

enum STATE
{
  STATE_DEFAULT,
  STATE_INVENTORY,
  STATE_PICKUP,
  STATE_WIELD,
  STATE_WEAR,
  STATE_DROP
};

typedef std::vector<std::pair<int, Item*> >::iterator InventoryIterator;

class Player
{
private:
	std::string name;
	Creature* creature;
	Skill skills[NUM_SKILL];
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
};

#endif