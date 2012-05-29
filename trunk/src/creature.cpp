#include "creature.hpp"

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