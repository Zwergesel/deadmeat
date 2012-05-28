#include "creature.hpp"

Creature::Creature(int x, int y, std::string name, int symbol, TCODColor color)
	:x(x), y(y), name(name), symbol(symbol), color(color)
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

int Creature::getX()
{
	return x;
}

int Creature::getY()
{
	return y;
}

int Creature::getSymbol()
{
	return symbol;
}

TCODColor Creature::getColor()
{
	return color;
}

void Creature::move(int dx, int dy)
{
	x += dx;
	y += dy;
}

void Creature::moveTo(int x, int y)
{
	this->x = x;
	this->y = y;
}