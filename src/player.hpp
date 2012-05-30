#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <libtcod.hpp>
#include <string>
#include <map>
#include "utility.hpp"

class Creature;
class Level;
class Item;

enum ATTRIBUTE
{
  ATTR_STR,
  ATTR_DEX,
  ATTR_CON,
  ATTR_INT
};

enum SKILLS
{
  SKILL_MELEE_COMBAT = 0,
  SKILL_RANGED_COMBAT,
  SKILL_HEALTH,
  SKILL_UNARMORED,
  SKILL_CLOTH_ARMOR,
  SKILL_LEATHER_ARMOR,
  SKILL_RING_ARMOR,
  SKILL_SCALE_ARMOR,
  SKILL_PLATE_ARMOR,
  SKILL_UNARMED,
  SKILL_AXE,
  SKILL_SWORD,
  SKILL_MACEFLAIL,
  SKILL_STAFF,
  SKILL_DAGGER,
  SKILL_WHIP,
  SKILL_PIKE,
  SKILL_BOW,
  SKILL_CROSSBOW,
  SKILL_SLING,
  NUM_SKILL
};

struct Skill
{
	Skill();
	Skill(std::string name, int value, ATTRIBUTE att);
	std::string name;
	int value;
	int exp;
	ATTRIBUTE att;
	int used;
};

class Player
{
private:
	std::string name;
	Creature* creature;
	Skill skills[NUM_SKILL];
	Item** inventory;
  // negative if inv is closed, otherwise the page number the inv is open on
	int inventoryOpen;

public:
	static int dx[9];
	static int dy[9];

	Player(std::string name);
	~Player();
	std::string getName();
	Creature* getCreature();
	int action(Level* level);
	TCOD_key_t waitForKeypress(bool clBuf);
	bool addItem(Item* i);
	void removeItem(Item* i);
	Item** getInventory();
	int isInventoryOpen();
};

#endif