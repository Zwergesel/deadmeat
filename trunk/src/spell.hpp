#ifndef _SPELL_HPP
#define _SPELL_HPP

#include "skill.hpp"
#include "utility.hpp"

enum SPELL
{
  SPELL_V_CRIPPLE,
  SPELL_V_FRENZY,
  SPELL_V_INNER_DEMONS,
  SPELL_V_FEAST,
  SPELL_V_RAISE_UNDEAD,
  SPELL_V_TRANCE,
  SPELL_V_SOUL_SWITCH,
  SPELL_E_FIRE,
  SPELL_P_FORCE_FIELD,
  SPELL_P_GUARDIAN_SHIELD,
  SPELL_P_BLINK,
  SPELL_P_FORCE_CAGE,
  SPELL_P_CHRONO_BOOST,
  SPELL_P_RECALL,
  SPELL_P_INVISIBILITY,
  NUM_SPELL
};

class Spell
{
private:
	std::string name;
	SKILLS magicSchool;
	int manaCost;
	int castTime;
	int difficulty;
	bool targeted;
	bool projectile;

public:
	Spell(std::string name, SKILLS magicSchool, int manaCost, int castTime, int difficulty, bool targeted, bool projectile);
	std::string getName();
	SKILLS getSkill();
	int getManaCost();
	int getCastTime();
	int getDifficulty();
	bool isTargeted();
	bool isProjectile();
	static bool cast(SPELL spell, Creature* caster, Point target);
};

extern Spell g_spells[NUM_SPELL];

#endif