#include <libtcod.hpp>
#include <sstream>
#include "weapon.hpp"

Weapon::Weapon() :Item(Point(0,0), "default", '§', TCODColor::pink), speed(0), hitBonus(0), baseDamage(0), numDice(0)
	,diceMax(0), enchantment(0), skill(SKILL_UNARMED), hands(2)
{
	type = ITEM_WEAPON;
	strType = "weapon";
}

Weapon::Weapon(Point position, std::string name, int symbol, TCODColor color, int spd, int hit, int dmg, int dice, int dmax, int ench, SKILLS skl, int hands)
	:Item(position, name, symbol, color), speed(spd), hitBonus(hit), baseDamage(dmg), numDice(dice)
	,diceMax(dmax), enchantment(ench), skill(skl), hands(hands)
{
	type = ITEM_WEAPON;
	strType = "weapon";
}

int Weapon::rollDamage()
{
	TCODRandom* rng = TCODRandom::getInstance();
	rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);
	int dmg = baseDamage;
	for (int i=numDice; i>0; --i)
	{
		dmg += rng->getInt(0, diceMax);
	}
	return dmg;
}

int Weapon::getMinDamage()
{
	return baseDamage;
}

int Weapon::getMaxDamage()
{
	return baseDamage + numDice * diceMax;
}

int Weapon::getSpeed()
{
	return speed;
}

int Weapon::getHitBonus()
{
	return hitBonus;
}

int Weapon::getEnchantment()
{
	return enchantment;
}

int Weapon::getHandsUsed()
{
	return hands;
}

std::string Weapon::toString()
{
	std::stringstream ss;
	int e = getEnchantment();
	ss << (e < 0 ? "" : "+") << e << " " << getName();
	return ss.str();
}

SKILLS Weapon::getSkill()
{
	return skill;
}