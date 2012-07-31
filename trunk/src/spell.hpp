#ifndef _SPELL_HPP
#define _SPELL_HPP

#include "skill.hpp"
#include "utility.hpp"

enum SPELL
{
  SPELL_V_CRIPPLE,
  SPELL_V_FRENZY,
  SPELL_E_FIRE,
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