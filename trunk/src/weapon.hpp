#ifndef _WEAPON_H
#define _WEAPON_H

#include "player.hpp";

class Weapon
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
	Weapon(int spd, int hit, int dmg, int dice, int dmax, int ench, SKILLS skl, int hands);
	int rollDamage();
	int getMinDamage();
	int getMaxDamage();
	int getSpeed();
	int getHitBonus();
	int getEnchantment();
	int getHandsUsed();
};

#endif