#include "../creature.hpp"
#include "../chargen.hpp"
#include "../savegame.hpp"

PlayerCreature::PlayerCreature()
{
	// for savegames
}

PlayerCreature::PlayerCreature(PlayerClass c, PlayerRace r, Gender g) : 
  Creature(CharGen::RACE_NAMES[r], F_DEFAULT, (unsigned char)'@', TCODColor::black, 200, 75, 
    Weapon("fists", F_PLURAL, '#', TCODColor::pink, 1, 0, 8, 0, 3, 1, 2, 0, 2, EFFECT_NONE, 1, AMMO_NONE, -1), 0, 10, 0, ""),
	pclass(c), race(r), gender(g)
{	
	setControlled(true);
	setAttackSkill(0);
	setDefenseSkill(0);
}

void PlayerCreature::copyFrom(PlayerCreature* original)
{
	Creature::copyFrom(original);
	pclass = original->pclass;
	race = original->race;
	gender = original->gender;
}

void PlayerCreature::affect(Status type, int start, int duration, int strength)
{
	if (race == RACE_UNDEAD)
	{
		// Poison immunity
		if (type == STATUS_POISON) return;
	}
	Creature::affect(type, start, duration, strength);
}

/*--------------------- SAVING AND LOADING ---------------------*/

void PlayerCreature::storeAll(Savegame& sg, SaveBlock& store)
{
	Creature::storeAll(sg, store);
	store ("class", (int) pclass) ("race", (int) race) ("gender", (int) gender);
}

unsigned int PlayerCreature::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this, &id)) return id;
	SaveBlock store("PlayerCreature", id);
	storeAll(sg, store);
	sg << store;
	return id;
}

void PlayerCreature::load(LoadBlock& load)
{
	Creature::load(load);
	int c, r, g;
	load ("class", c) ("race", r) ("gender", g);
	if (c < 0 || c >= NUM_CLASS) throw SavegameFormatException("Playercreature::load _ class out of bounds");
	if (r < 0 || r >= NUM_RACE) throw SavegameFormatException("Playercreature::load _ race out of bounds");
	if (g < 0 || g >= NUM_GENDER) throw SavegameFormatException("Playercreature::load _ gender out of bounds");
	pclass = static_cast<PlayerClass>(c);
	race = static_cast<PlayerRace>(r);
	gender = static_cast<Gender>(g);
}