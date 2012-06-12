#ifndef _SKILL_HPP
#define _SKILL_HPP

#include <string>
#include <vector>
#include <algorithm>

enum ATTRIBUTE
{
  ATTR_STR,
  ATTR_DEX,
  ATTR_CON,
  ATTR_INT,
  NUM_ATTR
};

enum SKILLS
{
	SKILL_WEAPON_SPEED,
	SKILL_ATTACK,
	SKILL_DAMAGE,
	SKILL_DEFENSE,
	SKILL_ARMOR,
	SKILL_MOVE_SPEED,
	SKILL_RANGED_ATTACK,
	SKILL_HEALTH,
	SKILL_MANA,
	SKILL_MANA_REGEN,
	SKILL_NEG_EFFECT,
	SKILL_TRAPS,
	SKILL_ALCHEMY,
	SKILL_COOKING,
	SKILL_BLACKSMITH,
	NUM_SKILL
};

class Skill
{
public:
	std::string name;
	int value;
	int maxValue;
	int maxLevel;
	int exp;
	std::vector<std::vector<std::pair<int,int>>> req;
	Skill();
	Skill(std::string name, int value, int maxVal, int maxLevel, int exp);
	static int expNeeded(int currentValue);
};

#endif