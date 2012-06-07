#ifndef _SKILL_HPP
#define _SKILL_HPP

#include <string>

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
  static int expNeeded(int currentValue);
};

#endif