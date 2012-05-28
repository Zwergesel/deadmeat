#ifndef _CREATURE_HPP
#define _CREATURE_HPP

#include <libtcod.hpp>
#include <string>
#include <iostream>
#include <deque>
#include "level.hpp"
#include "player.hpp"
#include "utility.hpp"

class Level;

class Creature
{

protected:
	std::string name;
	Point position;
	int symbol;
	TCODColor color;

public:

	Creature(Point pos, std::string name, int symbol, TCODColor color);
	~Creature();
	std::string getName();
	Point getPos();
	int getSymbol();
	TCODColor getColor();
	void move(Point pos);
	void moveTo(Point pos);

	virtual int action(Level*, Player*);
};

class Goblin: public Creature
{
public:
	Goblin();
	int action(Level*,Player*);
};

class FailWhale: public Creature
{
public:
	FailWhale();
	int action(Level*,Player*);
};

#endif