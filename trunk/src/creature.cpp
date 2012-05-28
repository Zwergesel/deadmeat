#include "creature.hpp"

Creature::Creature(Point pos, std::string name, int symbol, TCODColor color)
	:position(pos), name(name), symbol(symbol), color(color)
{
}

Creature::~Creature()
{
	// nothing, override
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

int Creature::action(Level*, Player*, std::deque<std::string>*)
{
	return 0;
}