#include "creature.hpp"
#include <sstream>
#include "world.hpp"

Creature::Creature(Point p, std::string n, int s, TCODColor c, int h)
	:position(p), name(n), symbol(s), color(c), maxHealth(h), health(h)
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
	// ???
}

int Creature::action(Level*, Player*)
{
	return 1;
}

int Creature::getDefense()
{
	// armor + (fighting skill + armor skill)/2 + tile defender is on
	int defense = 0;
	return defense;
}

int Creature::attack(Player* player)
{
	// (weapon to hit + weapon enchantment) + ((fighting skill + weapon skill)/2)
	int attack = 10;
	// damage = (weapon damage + weapon enchantment)
	int damage = 10;
	int defense = player->getDefense();
	TCODRandom rngGauss;
	rngGauss.setDistribution(TCOD_DISTRIBUTION_GAUSSIAN_RANGE);
	int hit = rngGauss.getInt(-300,300,attack - defense);
	if (hit >= -70)
	{
		if (hit <= 0) damage /= 2;
		if (hit > 175) damage *= 2;
		std::stringstream msg;
		msg << "The " << name << " hits you for " << damage << ".";
		world.addMessage(msg.str());
		if (player->hurt(damage)) world.addMessage("You die!");

	}
	else
	{
		std::stringstream msg;
		msg << "The " << name << " misses you.";
		world.addMessage(msg.str());
	}
	// base attack time - weapon speed + armor hindrance
	return 10;
}