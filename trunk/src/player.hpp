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
  STATE_TAKEOFF,
  NUM_STATE
};

class Player
{
private:
	std::string name;
	Creature* creature;
	Skill skills[NUM_SKILL];
	int attributes[NUM_ATTR];
	static int dx[9];
	static int dy[9];
	STATE state;
	int actionMove(int direction);
	int actionLook(Point p);
	int actionPickup();
	int actionPickup(Item*);
	int actionWield(Item*);
	int actionWear(Item*);
	int actionTakeoff(Item*);
	int actionDrop();
	int actionDrop(Item*);
	int computeAttackBonus(Weapon* w);
	int computeArmorBonus(Armor* a);

public:
	Player();
	Player(std::string name);
	~Player();
	std::string getName();
	Creature* getCreature();
	int action();
	TCOD_key_t waitForKeypress(bool clBuf);
	STATE getState();
	int getAttribute(ATTRIBUTE attr);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif