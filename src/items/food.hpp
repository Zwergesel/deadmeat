#ifndef _FOOD_HPP
#define _FOOD_HPP

#include "../item.hpp"

class Savegame;
class LoadBlock;

class Food : public Item
{

private:
	int nutrition;

public:
	Food();
	Food(std::string name, uint format, symbol sym, TCODColor color, int nutrition);
	~Food();
	Item* clone();

	int getNutrition();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif