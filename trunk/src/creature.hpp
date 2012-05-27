#include <libtcod.hpp>
#include <string>
#include <iostream>
#include <deque>
#include "level.hpp"
#include "player.hpp"

#ifndef _CREATURE_HPP
#define _CREATURE_HPP

class Creature
{

protected:
	std::string name;
	int symbol;
	TCODColor color;

public:
	int x, y;

	Creature(int x, int y, std::string name, int symbol, TCODColor color);
	~Creature();
	virtual int action(Level*, Player, std::deque<std::string>*) = 0;
	void draw(int offsetX, int offsetY);
};

class Goblin: public Creature
{
public:
	Goblin();
	int action(Level*,Player,std::deque<std::string>*);
};

class FailWhale: public Creature
{
public:
	FailWhale();
	int action(Level*,Player,std::deque<std::string>*);
};

#endif