#ifndef _WEAPON_HPP
#define _WEAPON_HPP

#include "../player.hpp"
#include "../item.hpp"
#include <sstream>

class Savegame;
class LoadBlock;

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
	Weapon();
	Weapon(std::string name, symbol sym, TCODColor color, int spd, int hit, int dmg, int dice, int dmax, int ench, SKILLS skl, int hands);
	~Weapon();
	Item* clone();

	int rollDamage();
	int getMinDamage();
	int getMaxDamage();
	int getSpeed();
	int getHitBonus();
	int getEnchantment();
	int getHandsUsed();
	SKILLS getSkill();
	std::string toString();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif