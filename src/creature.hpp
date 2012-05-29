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
	int health, maxHealth;
	
	void die(Creature* instigator);

public:

	Creature(Point p, std::string n, int s, TCODColor c, int h);
	~Creature();
	std::string getName();
	Point getPos();
	int getSymbol();
	TCODColor getColor();
	void move(Point pos);
	void moveTo(Point pos);
	
	/* Hurt returns true if the creature was killed */
	bool hurt(int damage, Creature* instigator); // ,DamageType dt

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