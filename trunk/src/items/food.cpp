#include "food.hpp"
#include "savegame.hpp"

Food::Food()
{
	// empty constructor, for savegames
	type = ITEM_FOOD;
	strType = "food";
}

Food::Food(std::string n, uint f, symbol s, TCODColor c, int u, int t):
	Item(n,f,s,c,1), nutrition(u), eatTime(t)
{
	type = ITEM_FOOD;
	strType = "food";
}

Food::~Food() {}

Item* Food::clone()
{
	Food* copy = new Food(name, formatFlags, sym, color, nutrition, eatTime);
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
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color) ("amount", amount);
	store ("nutrition", nutrition);
	sg << store;
	return id;
}

void Food::load(LoadBlock& load)
{
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color) ("amount", amount);
	load ("nutrition", nutrition);
}