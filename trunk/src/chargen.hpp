#ifndef _CHARGEN_HPP
#define _CHARGEN_HPP

#include <string>
#include "utility.hpp"

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
  const static bool ClassRace[NUM_CLASS][NUM_RACE] = {
                        /* HUM DWA ELF ORC DRA GOL IMP UND */
    /*Warrior*/       {     1,  1,  1,  1,  1,  1,  0,  1},
    /*Rogue*/         {     1,  0,  1,  0,  1,  0,  1,  1},
    /*Hunter*/        {     1,  1,  1,  1,  1,  0,  0,  1},
    /*Paladin*/       {     1,  1,  0,  0,  1,  1,  0,  0},
    /*Monk*/          {     1,  1,  1,  0,  0,  1,  1,  0},
    /*Voodoo-Priest*/ {     1,  1,  0,  1,  0,  1,  1,  1},
    /*Druid*/         {     1,  1,  1,  1,  0,  0,  0,  0},
    /*Wizard*/        {     1,  0,  0,  1,  1,  1,  1,  1},
    /*Musketeer*/     {     1,  1,  0,  0,  1,  1,  1,  1},
    /*Alchemist*/     {     1,  1,  0,  0,  1,  1,  1,  1},
    /*Scholar*/       {     1,  1,  1,  0,  0,  0,  1,  1}
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
}

#endif