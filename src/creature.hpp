#ifndef _CREATURE_HPP
#define _CREATURE_HPP

#include <libtcod.hpp>
#include <string>
#include <sstream>
#include "utility.hpp"
#include "items/weapon.hpp"
#include "items/armor.hpp"

class Level;
class Player;
class Savegame;

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
	Armor* armor;
	int attackSkill;
	int armorSkill;
	Weapon baseWeapon;
	Armor baseArmor;
	void die(Creature* instigator);

public:

	Creature(); // for savegames
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
	Armor* getArmor();
	void wieldMainWeapon(Weapon* wpn, int attackSkill);
	void wearArmor(Armor* armor, int armorSkill);
	void setAttackSkill(int attackSkill);
	void setBaseWeapon(Weapon base);
	std::pair<int,int> getHealth();

	/* Hurt returns true if the creature was killed */
	bool hurt(int damage, Creature* instigator); // ,DamageType dt

	virtual int action();
	
	unsigned int save(Savegame* sg);
	void load(Savegame* sg, std::stringstream& ss);
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