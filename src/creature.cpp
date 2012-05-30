#include <sstream>
#include "creature.hpp"
#include "player.hpp"
#include "level.hpp"
#include "world.hpp"

Creature::Creature(Point p, std::string n, int s, TCODColor c, int h):
	name(n),
	position(p),
	symbol(s),
	color(c),
	health(h),
	maxHealth(h),
	controlled(false)
{
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
	// armor + (fighting skill + armor skill)/2 + tile defender is on
	int defense = 0;
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
	// (weapon to hit + weapon enchantment) + ((fighting skill + weapon skill)/2)
	int attack = 10;
	// damage = (weapon damage + weapon enchantment)
	int damage = 10;
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
	// base attack time - weapon speed + armor hindrance
	return 10;
}