#ifndef _ARMOR_HPP
#define _ARMOR_HPP

#include "../item.hpp"

class Savegame;
class LoadBlock;

enum ArmorSlot
{
  ARMOR_BODY,
  ARMOR_BOOTS,
  ARMOR_HAT,
  NUM_ARMOR_SLOTS
};

class Armor : public Item
{

private:
	int defense;
	int enchantment;
	int hindrance;
	ArmorSlot slot;

public:
	Armor();
	Armor(std::string name, uint format, symbol sym, TCODColor color, int amount, int defense, int enchantment, int hindrance, ArmorSlot as);
	~Armor();
	Item* clone();

	int getDefense();
	int getEnchantment();
	int getHindrance();
	ArmorSlot getSlot();
	std::string toString();
	void randomize(int level);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif