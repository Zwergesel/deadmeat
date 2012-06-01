#include <libtcod.hpp>
#include <sstream>
#include "weapon.hpp"
#include <iostream>
#include "savegame.hpp"

Weapon::Weapon() :Item("default", (unsigned char)'¤', TCODColor::pink), speed(0), hitBonus(0), baseDamage(0), numDice(0)
	,diceMax(0), enchantment(0), skill(SKILL_UNARMED), hands(2)
{
	type = ITEM_WEAPON;
	strType = "weapon";
}

Weapon::Weapon(std::string name, int symbol, TCODColor color, int spd, int hit, int dmg, int dice, int dmax, int ench, SKILLS skl, int hands)
	:Item(name, symbol, color), speed(spd), hitBonus(hit), baseDamage(dmg), numDice(dice)
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

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Weapon::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Weapon", id);
	store ("name", name) ("symbol", symbol) ("color", color) ("speed", speed);
	store ("hitBonus", hitBonus) ("baseDamage", baseDamage) ("numDice", numDice);
	store ("diceMax", diceMax) ("enchantment", enchantment) ("skill", (int)skill);
	store ("hands", hands);
	sg << store;
	return id;
}

void Weapon::load(Savegame* sg, std::stringstream& ss)
{
	name = sg->loadString("name", ss);
	symbol = sg->loadInt("symbol", ss);
	color = sg->loadColor("color", ss);
	speed = sg->loadInt("speed", ss);
	hitBonus = sg->loadInt("hitBonus", ss);
	baseDamage = sg->loadInt("baseDamage", ss);
	numDice = sg->loadInt("numDice", ss);
	diceMax = sg->loadInt("diceMax", ss);
	enchantment = sg->loadInt("enchantment", ss);
	// TODO : check range 0 - SKILL_MAX(?)
	skill = static_cast<SKILLS>(sg->loadInt("skill", ss));
	hands = sg->loadInt("hands", ss);
}