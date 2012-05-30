#ifndef _WEAPON_H
#define _WEAPON_H

#include "../player.hpp"
#include "../item.hpp"

class Weapon : public Item
{
private:
	int speed;
	int hitBonus;
	int baseDamage;
	int numDice;
	int diceMax; // dice go from 0 to diceMax
	int enchantment;
	SKILLS skill;
	int hands;  

public:
	Weapon(Point position, std::string name, int symbol, TCODColor color, int spd, int hit, int dmg, int dice, int dmax, int ench, SKILLS skl, int hands);
	int rollDamage();
	int getMinDamage();
	int getMaxDamage();
	int getSpeed();
	int getHitBonus();
	int getEnchantment();
	int getHandsUsed();
};

#endif