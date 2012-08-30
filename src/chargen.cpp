#include "chargen.hpp"

static std::string RACE_NAMES[NUM_RACE] = { "Human", "Dwarf", "Elf", "Orc", "Dragonborn", "Golem", "Imp", "Undead" };
static std::string CLASS_NAMES[NUM_CLASS] = { "Warrior", "Rogue", "Hunter", "Paladin", "Monk", "Voodoo-Priest",
											"Druid", "Sorcerer", "Musketeer", "Alchemist", "Scholar" };

Creature* CharGen::generate()
{
	Class c = CharGen::choose_class();
	Race r = CharGen::choose_race(c);
	Gender g = CharGen::choose_gender();
	return new PlayerCreature(c,r,g);
}

Class CharGen::choose_class()
{
	return CLASS_WARRIOR;
}

Race CharGen::choose_race(Class c)
{
	return RACE_HUMAN;
}