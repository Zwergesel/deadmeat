#include <sstream>
#include <iostream>
#include "creature.hpp"
#include "player.hpp"
#include "level.hpp"
#include "world.hpp"
#include "items/weapon.hpp"
#include "savegame.hpp"

Creature::Creature()
{
	// for savegames
}

Creature::Creature(Point p, std::string n, int s, TCODColor c, int h):
	name(n),
	position(p),
	symbol(s),
	color(c),
	health(h),
	maxHealth(h),
	controlled(false),
	mainWeapon(NULL),
	armor(NULL)
{
	baseWeapon = Weapon("hands", (unsigned char)'¤', TCODColor::pink, 10, 10, 10, 0, 0, 0, SKILL_UNARMED, 2);
	baseArmor = Armor("skin", (unsigned char)'¤', TCODColor::pink, 0, 0, SKILL_UNARMORED);
	attackSkill = 0;
	armorSkill = 0;
}

Creature::~Creature()
{
	// nothing
}

std::string Creature::getName()
{
	return name;
}

Point Creature::getPos()
{
	return position;
}

int Creature::getSymbol()
{
	return symbol;
}

TCODColor Creature::getColor()
{
	return color;
}

Weapon* Creature::getMainWeapon()
{
	return mainWeapon;
}

Armor* Creature::getArmor()
{
	return armor;
}

void Creature::wieldMainWeapon(Weapon* wpn, int skill)
{
	mainWeapon = wpn;
	attackSkill = skill;
}

void Creature::wearArmor(Armor* a, int skill)
{
	armor = a;
	armorSkill = skill;
}

void Creature::move(Point dpos)
{
	position.x += dpos.x;
	position.y += dpos.y;
}

void Creature::moveTo(Point pos)
{
	this->position = pos;
}

std::pair<int,int> Creature::getHealth()
{
	return std::make_pair(health, maxHealth);
}

bool Creature::hurt(int damage, Creature* instigator)
{
	health -= damage;
	if (health <= 0)
	{
		die(instigator);
		return true;
	}
	return false;
}

void Creature::die(Creature* instigator)
{
	if (controlled)
	{
		world.addMessage("You die...");
		world.addMessage("Suddenly the amulet around your neck begins to glow brightly...", true);
		world.addMessage("You feel better!", true);
		health = maxHealth;
	}
	else
	{
		std::stringstream msg;
		instigator->isControlled() ? (msg << "You kill") : (msg << "The " << instigator->getName() << " kills");
		msg << " the " << name << ".";
		world.addMessage(msg.str());
		level->removeCreature(this, true);
	}
}

int Creature::action()
{
	return 1;
}

int Creature::getDefense()
{
	// armor + (fighting skill + armor skill)/2
	int defense = baseArmor.getAC() + armorSkill;
	if (armor != NULL)
	{
		defense = armor->getAC() + armorSkill;
	}
	return defense;
}

bool Creature::isControlled()
{
	return controlled;
}

void Creature::setControlled(bool c)
{
	controlled = c;
}

void Creature::setLevel(Level* l)
{
	level = l;
}

int Creature::attack(Creature* target)
{
	// base attack (hands, claws, etc.)
	int attack = baseWeapon.getHitBonus() + baseWeapon.getEnchantment() + attackSkill;
	// base attack damage
	int damage = baseWeapon.rollDamage();
	// base attack speed
	int speed = baseWeapon.getSpeed() - 0;

	if (mainWeapon != NULL)
	{
		// (weapon to hit + weapon enchantment) + ((fighting skill + weapon skill)/2)
		attack = mainWeapon->getHitBonus() + mainWeapon->getEnchantment() + attackSkill;
		// damage = (weapon damage + weapon enchantment)
		damage = mainWeapon->rollDamage();
		// weapon speed + armor hindrance
		speed = mainWeapon->getSpeed() - 0;
	}
	int defense = target->getDefense();
	TCODRandom rngGauss;
	rngGauss.setDistribution(TCOD_DISTRIBUTION_GAUSSIAN_RANGE);
	int mean = attack - defense;
	int hit = 0;
	if (mean >= 0) hit = rngGauss.getInt(-300, 300, mean);
	if (mean < 0) hit = -rngGauss.getInt(-300, 300, -mean);
	if (hit >= -70)
	{
		if (hit <= 0) damage /= 2;
		if (hit > 175) damage *= 2;
		std::stringstream msg;
		controlled ? (msg << "You hit ") : (msg << "The " << name << " hits ");
		target->isControlled() ? (msg << "you for ") : (msg << "the " << target->getName() << " for ");
		msg << damage << " damage.";
		world.addMessage(msg.str());
		target->hurt(damage, this);
	}
	else
	{
		std::stringstream msg;
		controlled ? (msg << "You miss ") : (msg << "The " << name << " misses ");
		target->isControlled() ? (msg << "you.") : (msg << "the " << target->getName() << ".");
		world.addMessage(msg.str());
	}

	return speed;
}

void Creature::setAttackSkill(int attack)
{
	attackSkill = attack;
}

void Creature::setBaseWeapon(Weapon base)
{
	baseWeapon = base;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Creature::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this, &id)) return id;
	SaveBlock store("Creature", id);
	store ("name", name) ("symbol", symbol) ("position", position);
	store ("color", color) ("health", health) ("maxHealth", maxHealth);
	store ("controlled", controlled);
	store.ptr("mainWeapon", mainWeapon == NULL ? 0 : mainWeapon->save(sg));
	store.ptr("armor", armor == NULL ? 0 : armor->save(sg));
	store ("attackSkill", attackSkill) ("armorSkill", armorSkill);
	store.ptr("baseWeapon", baseWeapon.save(sg)).ptr("baseArmor", baseArmor.save(sg));
	sg << store;
	return id;
}

void Creature::load(Savegame* sg, std::stringstream& ss)
{
	name = sg->loadString("name", ss);
	symbol = sg->loadInt("symbol", ss);
	position = sg->loadPoint("position", ss);
	color = sg->loadColor("color", ss);
	health = sg->loadInt("health", ss);
	maxHealth = sg->loadInt("maxHealth", ss);
	controlled = sg->loadBool("controlled", ss);
	mainWeapon = static_cast<Weapon*>(sg->loadPointer("mainWeapon", ss));
	armor = static_cast<Armor*>(sg->loadPointer("armor", ss));
	attackSkill = sg->loadInt("attackSkill", ss);
	armorSkill = sg->loadInt("armorSkill", ss);
	baseWeapon = *static_cast<Weapon*>(sg->loadPointer("baseWeapon", ss));
	baseArmor = *static_cast<Armor*>(sg->loadPointer("baseArmor", ss));
}