#include "creature.hpp"
#include "chargen.hpp"

PlayerCreature::PlayerCreature(PlayerClass c, PlayerRace r, Gender g) : 
  Creature(CharGen::RACE_NAMES[r], F_DEFAULT, (unsigned char)'@', TCODColor::black, 200, 75, 
    Weapon("fists", F_PLURAL, '#', TCODColor::pink, 1, 0, 8, 0, 3, 1, 2, 0, 2, EFFECT_NONE, 1, AMMO_NONE, -1), 0, 10, 0, "")
{	
	setControlled(true);
	setAttackSkill(0);
	setDefenseSkill(0);
}