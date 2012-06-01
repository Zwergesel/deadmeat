#ifndef _ARMOR_HPP
#define _ARMOR_HPP

#include "../player.hpp"
#include "../item.hpp"

class Armor : public Item
{
private:
	int ac;
	int hindrance;
	SKILLS skill;
public:
	Armor();
	Armor(std::string name, int symbol, TCODColor color, int ac, int hindrance, SKILLS skill);
	int getAC();
	int getHindrance();
	SKILLS getSkill();
};

#endif