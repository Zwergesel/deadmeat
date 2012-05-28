#ifndef _CREATURE_HPP
#define _CREATURE_HPP

#include <libtcod.hpp>
#include <string>
#include <iostream>
#include <deque>
#include "level.hpp"
#include "player.hpp"
#include "utility.hpp"

class Creature
{

protected:
	std::string name;
	int x, y;
	int symbol;
	TCODColor color;

public:

	Creature(int x, int y, std::string name, int symbol, TCODColor color);
	~Creature();
	std::string getName();
	int getX();
	int getY();
	int getSymbol();
	TCODColor getColor();
	void move(int dx, int dy);
	void moveTo(int x, int y);
	
	virtual int action(Level*, Player*, std::deque<std::string>*) = 0;
};

class Goblin: public Creature
{
public:
	Goblin();
	int action(Level*,Player*,std::deque<std::string>*);
};

class FailWhale: public Creature
{
public:
	FailWhale();
	int action(Level*,Player*,std::deque<std::string>*);
};

#endif