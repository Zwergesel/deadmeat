#ifndef _FOOD_HPP
#define _FOOD_HPP

#include "../item.hpp"

class Savegame;
class LoadBlock;

class Food : public Item
{

private:
	int nutrition;
	int eatTime;

public:
	Food();
	Food(Name name, symbol sym, TCODColor color, int amount, int weight, int nutrition, int eatTime);
	~Food();
	Item* clone();

	int getNutrition();
	int getEatTime();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif