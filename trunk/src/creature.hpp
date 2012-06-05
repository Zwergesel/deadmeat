#ifndef _CREATURE_HPP
#define _CREATURE_HPP

#include <libtcod.hpp>
#include <string>
#include <sstream>
#include <map>
#include "utility.hpp"
#include "items/weapon.hpp"
#include "items/armor.hpp"

class Level;
class Player;
class Savegame;
class LoadBlock;

class Creature
{

protected:
	std::string name;
	Point position;
	symbol sym;
	TCODColor color;
	int health, maxHealth;
	bool controlled;
	Level* level;
	symbol mainWeapon;
	symbol armor[NUM_ARMOR_SLOTS];
	int attackSkill;
	int armorSkill;
	Weapon baseWeapon;
	Armor baseArmor;
	std::map<symbol, Item*> inventory;
	void die(Creature* instigator);
	Creature(const Creature& copy);

public:

	Creature(); // for savegames
	Creature(Point p, std::string name, symbol s, TCODColor color, int health);
	~Creature();
	virtual Creature* clone();
	std::string getName();
	Point getPos();
	int getSymbol();
	TCODColor getColor();
	int attack(Creature* c);
	void move(Point pos);
	void moveTo(Point pos);
	int getDefense();
	int getAttack();
	bool isControlled();
	void setControlled(bool);
	void setLevel(Level* l);
	Weapon* getMainWeapon();
	Armor* getArmor(ArmorSlot slot);
	void wieldMainWeapon(Weapon* wpn, int attackSkill);
	void wearArmor(Armor* armor, int armorSkill);
	void setAttackSkill(int attackSkill);
	void setBaseWeapon(Weapon base);
	bool addItem(Item* item);
	void removeItem(Item* item, bool del);
	std::map<symbol, Item*> getInventory();
	std::pair<int,int> getHealth();

	/* Hurt returns true if the creature was killed */
	bool hurt(int damage, Creature* instigator); // ,DamageType dt

	virtual int action();

	virtual unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

class Goblin: public Creature
{
public:
	Goblin();
	Goblin(Point p, std::string n, int s, TCODColor t, int h);
	Creature* clone();
	int action();
	unsigned int save(Savegame& sg);
};

#endif