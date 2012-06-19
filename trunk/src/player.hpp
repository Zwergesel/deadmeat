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
  STATE_RANGED_ATTACK,
  STATE_EAT,
  NUM_STATE
};

enum HungerLevel
{
  HUNGER_STARVING = 0,
  HUNGER_WEAK = 600,
  HUNGER_HUNGRY = 1500,
  HUNGER_NORMAL = 3500,
  HUNGER_SATIATED = 4500
};

class Player
{
private:
	std::string name;
	Creature* creature;
	Skill skills[NUM_SKILL];
	int attributes[NUM_ATTR];
	STATE state;
	Point cursor;
	int level;
	int nutrition;
	int experience;
	int attrPoints;
	int skillPoints;
	std::vector<Creature*> targetList;

	static const std::string HELP_TEXT;
	static int dx[9];
	static int dy[9];

	int actionMove(int direction);
	int actionLook(Point p);
	int actionPickup();
	int actionPickup(Item*, int num);
	int actionWield(Item*);
	int actionWear(Item*);
	int actionTakeoff(Item*);
	int actionEat(Item*);
	int actionDrop();
	int actionDrop(Item*, int num);
	int actionRangedAttack(Point p);
	void actionAutoTargetting();
	int actionCharInfo(TCOD_key_t key);
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
	int rollBonusDamage();
	float getWeaponSpeedBonus();
	float getMoveSpeedBonus();
	float getArmorHindranceReduction();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif