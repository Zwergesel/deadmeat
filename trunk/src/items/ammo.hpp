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
	AmmoType ammoType;

public:
	Ammo();
	Ammo(Name name, symbol sym, TCODColor color, int amount, int weight, int enchantment, WeaponEffect effect, AmmoType ammoType);
	~Ammo();
	Item* clone();

	int getEnchantment();
	AmmoType getAmmoType();
	WeaponEffect getEffect();
	std::string toString();
	void randomize(int level);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif