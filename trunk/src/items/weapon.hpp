#ifndef _WEAPON_HPP
#define _WEAPON_HPP

#include "../player.hpp"
#include "../item.hpp"

class Savegame;
class LoadBlock;

enum WeaponEffect
{
  EFFECT_NONE,
  EFFECT_POISON,
  EFFECT_FIRE,
  NUM_EFFECT
};

class Weapon : public Item
{
private:
	int speed;
	int hitBonus;
	int baseDamage;
	int numDice;
	int diceMax; // dice go from 0 to diceMax
	int enchantment;
	int hands;
	WeaponEffect effect;
	int range;

public:
	Weapon();
	Weapon(std::string name, uint format, symbol sym, TCODColor color, int spd, int hit, int dmg, int dice, int dmax, int ench, int hands, WeaponEffect effect, int range);
	Weapon(int spd, int hit, int dmg, int dice, int dmax, WeaponEffect effect, int range);
	~Weapon();
	Item* clone();

	int rollDamage();
	int getMinDamage();
	int getMaxDamage();
	int getSpeed();
	int getHitBonus();
	int getEnchantment();
	int getHandsUsed();
	int getRange();
	std::string toString();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif