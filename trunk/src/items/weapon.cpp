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

Weapon::Weapon(std::string n, uint f, symbol s, TCODColor c, int spd, int hit, int dmg, int dice, int dmax, int ench, int h, WeaponEffect e, int r)
	:Item(n, f, s, c), speed(spd), hitBonus(hit), baseDamage(dmg), numDice(dice)
	,diceMax(dmax), enchantment(ench), hands(h), effect(e), range(r)
{
	type = ITEM_WEAPON;
	strType = "weapon";
}

Weapon::~Weapon() {}

Item* Weapon::clone()
{
	Weapon* copy = new Weapon(name, formatFlags, sym, color, speed, hitBonus, baseDamage, numDice, diceMax, enchantment, hands, effect, range);
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

int Weapon::getRange()
{
	return range;
}

std::string Weapon::toString()
{
	std::stringstream ss;
	ss << (enchantment < 0 ? "" : "+") << enchantment << " " << name;
	return ss.str();
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Weapon::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Weapon", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color) ("speed", speed);
	store ("hitBonus", hitBonus) ("baseDamage", baseDamage) ("numDice", numDice);
	store ("diceMax", diceMax) ("enchantment", enchantment);
	store ("hands", hands) ("effect", effect) ("range", range);
	sg << store;
	return id;
}

void Weapon::load(LoadBlock& load)
{
	int e;
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color) ("speed", speed);
	load ("hitBonus", hitBonus) ("baseDamage", baseDamage) ("numDice", numDice);
	load ("diceMax", diceMax) ("enchantment", enchantment) ("hands", hands) ("effect", e);
	load ("range", range);
	if (e < 0 || e >= NUM_EFFECT) throw SavegameFormatException("Weapon::load _ effect out of range");
	effect = static_cast<WeaponEffect>(e);
}