#include "food.hpp"
#include "savegame.hpp"
#include "../world.hpp"

Food::Food()
{
	// empty constructor, for savegames
	type = ITEM_FOOD;
	strType = "food";
}

Food::Food(Name n, symbol s, TCODColor c, int x, int w, int u, int t):
	Item(n,s,c,x,w), nutrition(u), eatTime(t)
{
	type = ITEM_FOOD;
	strType = "food";
}

Food::~Food() {}

Item* Food::clone()
{
	Food* copy = new Food(name, sym, color, amount, weight, nutrition, eatTime);
	copy->active = active;
	return copy;
}

int Food::getNutrition()
{
	return nutrition;
}

int Food::getEatTime()
{
	return eatTime;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Food::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Food", id);
	store ("name", name) ("symbol", sym) ("color", color);
	store ("amount", amount) ("weight", weight) ("active", active);
	store ("nutrition", nutrition) ("eatTime", eatTime);
	sg << store;
	return id;
}

void Food::load(LoadBlock& load)
{
	load ("name", name) ("symbol", sym) ("color", color);
	load ("amount", amount) ("weight", weight) ("active", active);
	load ("nutrition", nutrition) ("eatTime", eatTime);
}