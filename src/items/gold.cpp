#include "gold.hpp"
#include "../savegame.hpp"
#include "../world.hpp"

Gold::Gold():
	Item(Name("gold","gold"), '$', TCODColor(242, 210, 0), 1, 0)
{
	// constructor for savegames
	type = ITEM_GOLD;
	strType = "gold";
}

Gold::Gold(int x):
	Item(Name("gold","gold"), '$', TCODColor(242, 210, 0), x, 0)
{
	type = ITEM_GOLD;
	strType = "gold";
}

Gold::~Gold() {}

Item* Gold::clone()
{
	Gold* copy = new Gold(amount);
	copy->active = active;
	return copy;
}

void Gold::randomize(int level)
{
	amount = rng->getInt(0,250,30);
	if (amount == 0) amount = rng->getInt(1, 30);
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Gold::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Gold", id);
	store ("name", name) ("symbol", sym) ("color", color);
	store ("amount", amount) ("weight", weight) ("active", active);
	sg << store;
	return id;
}

void Gold::load(LoadBlock& load)
{
	load ("name", name) ("symbol", sym) ("color", color);
	load ("amount", amount) ("weight", weight) ("active", active);
}