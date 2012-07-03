#include <libtcod.hpp>
#include <sstream>
#include "weapon.hpp"
#include <iostream>
#include "../savegame.hpp"
#include "../world.hpp"

std::string Weapon::EFFECT_NAMES[NUM_EFFECT] = { "", "poisoned", "flaming", "contaminated" };

Weapon::Weapon()
{
	// Empty constructor, for savegames
	type = ITEM_WEAPON;
	strType = "weapons";
}

Weapon::Weapon(std::string n, uint f, symbol s, TCODColor c, int x, int w, int spd, int hit, int dmg, int dice, int dmax, int ench, int h, WeaponEffect e, int r, AmmoType a)
	:Item(n,f,s,c,x,w), speed(spd), hitBonus(hit), baseDamage(dmg), numDice(dice)
	,diceMax(dmax), enchantment(ench), hands(h), effect(e), range(r), ammoType(a)
{
	type = ITEM_WEAPON;
	strType = "weapons";
}

Weapon::Weapon(int spd, int hit, int dmg, int dice, int dmax, WeaponEffect e, int r, AmmoType a):
	Item("x", F_DEFAULT, '#', TCODColor::pink, 1, 0), speed(spd), hitBonus(hit), baseDamage(dmg),
	numDice(dice), diceMax(dmax), enchantment(0), hands(0), effect(e), range(r), ammoType(a)
{
	type = ITEM_WEAPON;
	strType = "weapons";
}

Weapon::~Weapon() {}

Item* Weapon::clone()
{
	Weapon* copy = new Weapon(name, formatFlags, sym, color, amount, weight, speed, hitBonus, baseDamage, numDice, diceMax, enchantment, hands, effect, range, ammoType);
	copy->active = active;
	return copy;
}

int Weapon::rollDamage()
{
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

float Weapon::getDPS()
{
	return 10.f * (baseDamage + numDice * diceMax/2.0f) / speed;
}

AmmoType Weapon::getAmmoType()
{
	return ammoType;
}

std::string Weapon::toString()
{
	std::stringstream ss;
	if (EFFECT_NAMES[effect].length() > 0) ss << EFFECT_NAMES[effect] << " ";
	ss << (enchantment < 0 ? "" : "+") << enchantment << " ";
	ss << name;
	return ss.str();
}

void Weapon::randomize(int level)
{
	int roll = rng->getInt(0,999);
	enchantment = (roll / 800) + (roll / 950) + (roll / 995);
	roll = rng->getInt(0,999);
	if (roll < 200) effect = EFFECT_FIRE;
	else if (roll < 400) effect = EFFECT_POISON;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Weapon::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Weapon", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	store ("color", color) ("amount", amount) ("weight", weight) ("active", active);
	store ("speed", speed) ("hitBonus", hitBonus) ("baseDamage", baseDamage);
	store ("numDice", numDice) ("diceMax", diceMax) ("enchantment", enchantment);
	store ("hands", hands) ("effect", effect) ("range", range);
	store ("ammoType", ammoType);
	sg << store;
	return id;
}

void Weapon::load(LoadBlock& load)
{
	int e,a;
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	load ("color", color) ("amount", amount) ("weight", weight) ("active", active);
	load ("speed", speed) ("hitBonus", hitBonus) ("baseDamage", baseDamage);
	load ("numDice", numDice) ("diceMax", diceMax) ("enchantment", enchantment);
	load ("hands", hands) ("effect", e) ("range", range);
	load ("ammoType", a);
	if (a < 0 || a >= NUM_AMMOTYPE) throw SavegameFormatException("Weapon::load _ ammoType out of range");
	ammoType = static_cast<AmmoType>(a);
	if (e < 0 || e >= NUM_EFFECT) throw SavegameFormatException("Weapon::load _ effect out of range");
	effect = static_cast<WeaponEffect>(e);
}