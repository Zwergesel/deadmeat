#include "chargen.hpp"
#include "creature.hpp"

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