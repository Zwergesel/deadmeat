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
  STATE_INSPECT,
  STATE_CHARINFO,
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
	Point cursor;
	int level;
	int nutrition;
	int experience;
	int attrPoints;
	int skillPoints;
	int actionMove(int direction);
	int actionLook(Point p);
	int actionPickup();
	int actionPickup(Item*);
	int actionWield(Item*);
	int actionWear(Item*);
	int actionTakeoff(Item*);
	int actionDrop();
	int actionDrop(Item*);
	int actionCharInfo(TCOD_key_t key);
	int computeAttackBonus(Weapon* w);
	int computeArmorBonus(Armor* a);
	void moveCursor(int dir);

public:
	Player();
	Player(std::string name);
	~Player();
	std::string getName();
	Creature* getCreature();
	Point getCursor();
	void addNutrition(int delta);
	int getNutrition();
	int action();
	int processAction();
	TCOD_key_t waitForKeypress(bool clBuf);
	STATE getState();
	int getAttribute(ATTRIBUTE attr);
	Skill getSkill(SKILLS skill);
	void incExperience(int exp);
	int getLevel();
	int getExperience();
	int getNeededExp();
	void levelUp();
	int getAttributePoints();
	int getSkillPoints();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif