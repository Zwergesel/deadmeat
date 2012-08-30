#include "chargen.hpp"

static std::string RACE_NAMES[NUM_RACE] = { "Human", "Dwarf", "Elf", "Orc", "Dragonborn", "Golem", "Imp", "Undead" };
static std::string CLASS_NAMES[NUM_CLASS] = { "Warrior", "Rogue", "Hunter", "Paladin", "Monk", "Voodoo-Priest",
											"Druid", "Sorcerer", "Musketeer", "Alchemist", "Scholar" };

Creature* CharGen::generate()
{
	PlayerClass c = CharGen::choose_class();
	PlayerRace r = CharGen::choose_race(c);
	Gender g = CharGen::choose_gender();
	return new PlayerCreature(c,r,g);
}

PlayerClass CharGen::choose_class()
{
	return CLASS_WARRIOR;
}

PlayerRace CharGen::choose_race(PlayerClass c)
{
	return RACE_HUMAN;
}

Gender CharGen::choose_gender()
{
	return GENDER_MALE;
}