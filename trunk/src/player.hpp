#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <string>
#include <vector>
#include <libtcod.hpp>
#include "utility.hpp"

enum ATTRIBUTE
{
  ATTR_STR,
  ATTR_DEX,
  ATTR_CON
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
};

class Player
{
private:
	Point position;
	std::string name;
	int symbol;
	TCODColor color;
	int actionTime;
	Skill skills[NUM_SKILL];
	int health;
	int maxHealth;

public:
	static int dx[9];
	static int dy[9];

	Player(std::string name);
	std::string getName();
	Point getPos();
	int getSymbol();
	TCODColor getColor();
	int getActionTime();
	void move(Point dpos);
	void moveTo(Point p);
	void attack(int& attack, int& damage, int& speed);
	int getDefense();
	void addActionTime(int dt);
	bool hurt(int damage);
};

#endif