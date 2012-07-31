#include "spell.hpp"
#include "level.hpp"
#include "world.hpp"
#include "creature.hpp"
#include <cassert>

Spell g_spells[NUM_SPELL] =
{
	Spell("Cripple", SKILL_VOODOO, 10, 10, 10, true, false),
	Spell("Frenzy", SKILL_VOODOO, 20, 20, 20, false, false),
	Spell("Fire", SKILL_ELEMENTAL, 10, 10, 10, true, true)
};


Spell::Spell(std::string n, SKILLS s, int mc, int c, int d, bool t, bool p) : name(n), magicSchool(s), manaCost(mc), castTime(c), difficulty(d), targeted(t), projectile(p)
{
}

std::string Spell::getName()
{
	return name;
}

SKILLS Spell::getSkill()
{
	return magicSchool;
}

int Spell::getManaCost()
{
	return manaCost;
}

int Spell::getCastTime()
{
	return castTime;
}

int Spell::getDifficulty()
{
	return difficulty;
}

bool Spell::isTargeted()
{
	return targeted;
}

bool Spell::isProjectile()
{
	return projectile;
}

bool Spell::cast(SPELL spell, Creature* caster, Point target)
{
	Level* level = world.levels[world.currentLevel];
	assert(level != NULL);
	if (spell == SPELL_V_CRIPPLE)
	{
		Creature* c = level->creatureAt(target);
		if (c == NULL) return false;
		c->affect(STATUS_SLOW, 0, 600, 7);
		return true;
	}
	else if (spell == SPELL_V_FRENZY)
	{
		world.player->getCreature()->affect(STATUS_BERSERK, 0, 900, 5);
		return true;
	}
	else if (spell == SPELL_E_FIRE)
	{
		Creature* c = level->creatureAt(target);
		if (c == NULL) return false;
		c->hurt(50, caster, DAMAGE_FIRE);
		c->affect(STATUS_FIRE, 0, 30, 5);
		return true;
	}
	return false;
}