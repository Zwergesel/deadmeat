#include "corpse.hpp"
#include "savegame.hpp"
#include "../world.hpp"

Corpse::Corpse()
{
	// empty constructor, for savegames
	type = ITEM_CORPSE;
	strType = "corpses";
}

Corpse::Corpse(std::string n, uint f, symbol s, TCODColor c, int w, int u, int e, int r):
	Item(n,f,s,c,1,w), nutrition(u), eatTime(e), rotTime(r)
{
	type = ITEM_CORPSE;
	strType = "corpses";
}

Corpse::~Corpse() {}

Item* Corpse::clone()
{
	Corpse* copy = new Corpse(name, formatFlags, sym, color, weight, nutrition, eatTime, rotTime);
	copy->amount = amount;
	copy->active = active;
	return copy;
}

int Corpse::getNutrition()
{
	return nutrition;
}

int Corpse::getEatTime()
{
	return eatTime;
}

int Corpse::getRotTime()
{
	return rotTime;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Corpse::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Corpse", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	store ("color", color) ("amount", amount) ("weight", weight) ("active", active);
	store ("nutrition", nutrition) ("eatTime", eatTime) ("rotTime", rotTime);
	sg << store;
	return id;
}

void Corpse::load(LoadBlock& load)
{
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	load ("color", color) ("amount", amount) ("weight", weight) ("active", active);
	load ("nutrition", nutrition) ("eatTime", eatTime) ("rotTime", rotTime);
}