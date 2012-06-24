#include "gold.hpp"
#include "../savegame.hpp"

Gold::Gold():
	Item("gold", F_PLURAL, '$', TCODColor(242, 210, 0), 1)
{
	// constructor for savegames
	type = ITEM_GOLD;
	strType = "gold";
}

Gold::Gold(int x):
	Item("gold", F_PLURAL, '$', TCODColor(242, 210, 0), x)
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
	TCODRandom* rng = TCODRandom::getInstance();
	amount = rng->getInt(0,250,30);
	if (amount == 0) amount = rng->getInt(1, 30);
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Gold::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Gold", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	store ("color", color) ("amount", amount) ("active", active);
	sg << store;
	return id;
}

void Gold::load(LoadBlock& load)
{
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	load ("color", color) ("amount", amount) ("active", active);
}