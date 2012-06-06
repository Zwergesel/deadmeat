#include <libtcod.hpp>
#include <sstream>
#include "weapon.hpp"
#include <iostream>
#include "savegame.hpp"

Weapon::Weapon()
{
	// Empty constructor, for savegames
	type = ITEM_WEAPON;
	strType = "weapon";
}

Weapon::Weapon(std::string n, symbol s, TCODColor c, int spd, int hit, int dmg, int dice, int dmax, int ench, SKILLS skl, int hands)
	:Item(n, s, c), speed(spd), hitBonus(hit), baseDamage(dmg), numDice(dice)
	,diceMax(dmax), enchantment(ench), skill(skl), hands(hands)
{
	type = ITEM_WEAPON;
	strType = "weapon";
}

Weapon::~Weapon(){}

Item* Weapon::clone()
{
	Weapon* copy = new Weapon(name, sym, color, speed, hitBonus, baseDamage, numDice, diceMax, enchantment, skill, hands);
	return copy;
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

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Weapon::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Weapon", id);
	store ("name", name) ("symbol", sym) ("color", color) ("speed", speed);
	store ("hitBonus", hitBonus) ("baseDamage", baseDamage) ("numDice", numDice);
	store ("diceMax", diceMax) ("enchantment", enchantment) ("skill", (int)skill);
	store ("hands", hands);
	sg << store;
	return id;
}

void Weapon::load(LoadBlock& load)
{
	int s;
	load ("name", name) ("symbol", sym) ("color", color) ("speed", speed);
	load ("hitBonus", hitBonus) ("baseDamage", baseDamage) ("numDice", numDice);
	load ("diceMax", diceMax) ("enchantment", enchantment) ("skill", s);
	if (s < 0 || s >= NUM_SKILL) throw SavegameFormatException("Weapon::load _ skill out of range");
	skill = static_cast<SKILLS>(s);
	load ("hands", hands);
}