#ifndef _CREATURE_HPP
#define _CREATURE_HPP

#include <libtcod.hpp>
#include <string>
#include "utility.hpp"
#include "items/weapon.hpp"

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
	bool controlled;
	Level* level;
	Weapon* mainWeapon;
	int attackSkill;
	Weapon baseWeapon;
	void die(Creature* instigator);

public:

	Creature(Point p, std::string name, int symbol, TCODColor color, int health);
	~Creature();
	std::string getName();
	Point getPos();
	int getSymbol();
	TCODColor getColor();
	int attack(Creature* c);
	void move(Point pos);
	void moveTo(Point pos);
	int getDefense();
	bool isControlled();
	void setControlled(bool);
	void setLevel(Level* l);
	Weapon* getMainWeapon();
	void wieldMainWeapon(Weapon* wpn, int attackSkill);
	void setAttackSkill(int attackSkill);
	void setBaseWeapon(Weapon base);

	/* Hurt returns true if the creature was killed */
	bool hurt(int damage, Creature* instigator); // ,DamageType dt

	virtual int action();
};

class Goblin: public Creature
{
public:
	Goblin();
	int action();
};

class FailWhale: public Creature
{
public:
	FailWhale();
	int action();
};

#endif