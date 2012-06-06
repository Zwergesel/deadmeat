#ifndef _ARMOR_HPP
#define _ARMOR_HPP

#include "../player.hpp"
#include "../item.hpp"
#include <sstream>

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
	SKILLS skill;

public:
	Armor();
	Armor(std::string name, symbol sym, TCODColor color, int defense, int enchantment, int hindrance, ArmorSlot as, SKILLS skill);
	~Armor();
	Item* clone();

	int getDefense();
	int getEnchantment();
	int getHindrance();
	ArmorSlot getSlot();
	SKILLS getSkill();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif