#ifndef _CHARGEN_HPP
#define _CHARGEN_HPP

#include <string>

class Creature;

enum PlayerRace
{
	RACE_HUMAN,
	RACE_DWARF,
	RACE_ELF,
	RACE_ORC,
	RACE_DRAGON,
	RACE_GOLEM,
	RACE_IMP,
	RACE_UNDEAD,
	NUM_RACE
};

enum PlayerClass
{
	CLASS_WARRIOR,
	CLASS_ROGUE,
	CLASS_HUNTER,
	CLASS_PALADIN,
	CLASS_MONK,
	CLASS_VOODOO,
	CLASS_DRUID,
	CLASS_SORCERER,
	CLASS_MUSKETEER,
	CLASS_ALCHEMIST,
	CLASS_SCHOLAR,
	NUM_CLASS
};

enum Gender
{
	GENDER_MALE,
	GENDER_FEMALE,
	NUM_GENDER
};

namespace CharGen
{
	static std::string RACE_NAMES[NUM_RACE] = { "Human", "Dwarf", "Elf", "Orc", "Dragonborn", "Golem", "Imp", "Undead" };
	static std::string CLASS_NAMES[NUM_CLASS] = { "Warrior", "Rogue", "Hunter", "Paladin", "Monk", "Voodoo-Priest",
											"Druid", "Sorcerer", "Musketeer", "Alchemist", "Scholar" };
	Creature* generate();
	PlayerClass choose_class();
	PlayerRace choose_race(PlayerClass c);
	Gender choose_gender();
}

#endif