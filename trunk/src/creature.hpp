#ifndef _CREATURE_HPP
#define _CREATURE_HPP

#include <libtcod.hpp>
#include <string>
#include <iostream>
#include <deque>
#include <sstream>
#include "utility.hpp"

class Level;
class Player;

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

	Creature(Point p, std::string name, int symbol, TCODColor color, int health);
	~Creature();
	std::string getName();
	Point getPos();
	int getSymbol();
	TCODColor getColor();
	int attack(Player* player);
	void move(Point pos);
	void moveTo(Point pos);
	int getDefense();

	/* Hurt returns true if the creature was killed */
	bool hurt(int damage, Creature* instigator); // ,DamageType dt

	virtual int action(Level*, Player*);
};

class Goblin: public Creature
{
public:
	Goblin();
	int action(Level*, Player*);
};

class FailWhale: public Creature
{
public:
	FailWhale();
	int action(Level*, Player*);
};

#endif