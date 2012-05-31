#include <sstream>
#include <iostream>
#include "creature.hpp"
#include "player.hpp"
#include "level.hpp"
#include "world.hpp"
#include "items/weapon.hpp"

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
	baseWeapon = Weapon(Point(0,0), "hands", '§', TCODColor::pink, 10, 10, 10, 0, 0, 0, SKILL_UNARMED, 2);
	baseArmor = Armor(Point(0,0), "skin", '§', TCODColor::pink, 0, 0, SKILL_UNARMORED);
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

void Creature::wieldMainWeapon(Weapon* wpn, int attack)
{
	mainWeapon = wpn;
	attackSkill = attack;
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
	int hit = rngGauss.getInt(-300,300,attack - defense);
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