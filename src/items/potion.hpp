#ifndef _POTION_HPP
#define _POTION_HPP

#include "../item.hpp"
#include "../utility.hpp"

enum PotionType
{
  POTION_MINORHEAL,
  POTION_HEAL,
  POTION_FULLHEAL,
  POTION_HASTE,
  NUM_POTIONTYPE
};

class Potion : public Item
{
private:
	PotionType potion;

public:
	Potion();
	~Potion();
	Potion(std::string name, uint format, symbol sym, TCODColor color, int amount, int weight, PotionType type);
	Item* clone();

	void effect(Creature* c);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif