#include "corpse.hpp"
#include "savegame.hpp"
#include "../world.hpp"

Corpse::Corpse()
{
	// empty constructor, for savegames
	type = ITEM_CORPSE;
	strType = "corpses";
}

Corpse::Corpse(Name n, symbol s, TCODColor c, int w, int u, int e, int r):
	Item(n,s,c,1,w), nutrition(u), eatTime(e), rotTime(r)
{
	type = ITEM_CORPSE;
	strType = "corpses";
}

Corpse::~Corpse() {}

Item* Corpse::clone()
{
	Corpse* copy = new Corpse(name, sym, color, weight, nutrition, eatTime, rotTime);
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

bool Corpse::isRotten()
{
	return world.time > rotTime;
}

bool Corpse::isDecaying()
{
	return world.time > rotTime + 3000;
}

void Corpse::initRotTime()
{
	rotTime += world.time;
}

std::string Corpse::toString()
{
	std::string str = "";
	if (world.time > rotTime + 1000) str.append("rotten ");
	str.append(name.name);
	return str;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Corpse::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Corpse", id);
	store ("name", name) ("symbol", sym) ("color", color);
	store ("amount", amount) ("weight", weight) ("active", active);
	store ("nutrition", nutrition) ("eatTime", eatTime) ("rotTime", rotTime);
	sg << store;
	return id;
}

void Corpse::load(LoadBlock& load)
{
	load ("name", name) ("symbol", sym) ("color", color);
	load ("amount", amount) ("weight", weight) ("active", active);
	load ("nutrition", nutrition) ("eatTime", eatTime) ("rotTime", rotTime);
}