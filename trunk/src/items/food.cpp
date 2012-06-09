#include "food.hpp"
#include "savegame.hpp"

Food::Food()
{
	// empty constructor, for savegames
	type = ITEM_FOOD;
	strType = "food";
}

Food::Food(std::string n, uint f, symbol s, TCODColor c, int u):
	Item(n, f, s, c), nutrition(u)
{
	type = ITEM_FOOD;
	strType = "food";
}

Food::~Food() {}

Item* Food::clone()
{
	Food* copy = new Food(name, formatFlags, sym, color, nutrition);
	return copy;
}

int Food::getNutrition()
{
	return nutrition;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Food::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Food", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color);
	store ("nutrition", nutrition);
	sg << store;
	return id;
}

void Food::load(LoadBlock& load)
{
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color);
	load ("nutrition", nutrition);
}