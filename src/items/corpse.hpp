#ifndef _CORPSE_HPP
#define _CORPSE_HPP

#include "../item.hpp"

class Savegame;
class LoadBlock;

class Corpse : public Item
{

private:
	int nutrition;
	int eatTime;
	int rotTime;

public:
	Corpse();
	Corpse(Name name, symbol sym, TCODColor color, int weight, int nutrition, int eatTime, int rotTime);
	~Corpse();
	Item* clone();

	int getNutrition();
	int getEatTime();
	bool isRotten();
	bool isDecaying();
	void initRotTime();

	std::string toString();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif