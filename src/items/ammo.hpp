#ifndef _AMMO_HPP
#define _AMMO_HPP

#include "../item.hpp"
#include "weapon.hpp"

class Savegame;
class LoadBlock;

class Ammo : public Item
{
private:
	int enchantment;
	WeaponEffect effect;

public:
	Ammo();
	Ammo(std::string name, uint format, symbol sym, TCODColor color, int amount, int enchantment, WeaponEffect effect);
	~Ammo();
	Item* clone();

	int getEnchantment();
	WeaponEffect getEffect();
	std::string toString();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif