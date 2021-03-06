#ifndef _CHARGEN_HPP
#define _CHARGEN_HPP

#include <string>
#include <vector>
#include "utility.hpp"
#include "skill.hpp"

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
	const static bool ClassRace[NUM_CLASS][NUM_RACE] =
	{
		/*                     HUM DWA ELF ORC DRA GOL IMP UND */
		/*Warrior*/       {     1,  1,  1,  1,  1,  1,  0,  1 },
		/*Rogue*/         {     1,  0,  1,  0,  1,  0,  1,  1 },
		/*Hunter*/        {     1,  1,  1,  1,  1,  0,  0,  1 },
		/*Paladin*/       {     1,  1,  0,  0,  1,  1,  0,  0 },
		/*Monk*/          {     1,  1,  1,  0,  0,  1,  1,  0 },
		/*Voodoo-Priest*/ {     1,  1,  0,  1,  0,  1,  1,  1 },
		/*Druid*/         {     1,  1,  1,  1,  0,  0,  0,  0 },
		/*Wizard*/        {     1,  0,  0,  1,  1,  1,  1,  1 },
		/*Musketeer*/     {     1,  1,  0,  0,  1,  1,  1,  1 },
		/*Alchemist*/     {     1,  1,  0,  0,  1,  1,  1,  1 },
		/*Scholar*/       {     1,  1,  1,  0,  0,  0,  1,  1 }
	};
	const static int startAttr[NUM_RACE][NUM_ATTR] =
	{
		/*                   STR DEX CON INT */
		/*Human*/       {     4,  4,  4,  8 },
		/*Dwarf*/       {     6,  4,  7,  5 },
		/*Elf*/         {     3,  8,  2,  4 },
		/*Orc*/         {     8,  4,  6,  3 },
		/*Dragonborn*/  {     7,  5,  5,  4 },
		/*Golem*/       {     7,  2,  9,  2 },
		/*Imp*/         {     3,  4,  2,  9 },
		/*Undead*/      {     4,  6,  8,  3 }
	};
	const static int startHealthRace[NUM_RACE] =
	{ /* HUM DWA ELF ORC DRA GOL IMP UND */
		  35, 50, 25, 45, 35, 35, 20, 30
	};
	const static int startHealthClass[NUM_CLASS] =
	{ /* WAR ROG HUN PAL MON VOO DRU WIZ MUS ALC SCO */
		  20, 12, 16, 15, 10,  8, 15, 10, 14, 12,  6 
	};
	const static int startManaRace[NUM_RACE] =
	{ /* HUM DWA ELF ORC DRA GOL IMP UND */
		  10,  0, 20,  5, 10,  5, 25, 10
	};
	const static int startManaClass[NUM_CLASS] =
	{ /* WAR ROG HUN PAL MON VOO DRU WIZ MUS ALC SCO */
		   0, 10,  5, 25, 40, 40, 30, 50,  5, 25, 15 
	};

	static std::string RACE_NAMES[NUM_RACE] = { "Human", "Dwarf", "Elf", "Orc", "Dragonborn", "Golem", "Imp", "Undead" };
	static std::string CLASS_NAMES[NUM_CLASS] = { "Warrior", "Rogue", "Hunter", "Paladin", "Monk", "Voodoo-Priest",
	    "Druid", "Sorcerer", "Musketeer", "Alchemist", "Scholar"
	                                            };
	static std::string GENDER_NAMES[NUM_GENDER] = { "male", "female" };
	static Viewport view(8,2,63,45);

	void generate();
	PlayerClass choose_class();
	PlayerRace choose_race(PlayerClass c);
	Gender choose_gender(PlayerClass c, PlayerRace r);
	std::string choose_name(PlayerClass c, PlayerRace r, Gender g);
	void draw(PlayerClass c, PlayerRace r, Gender g, const std::string& name);
	char waitForChar();
	bool isNameChar(char x);
	void generateStartItems(Creature* pc, PlayerClass c, PlayerRace r, Gender g);
}

#endif