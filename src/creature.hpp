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
	static const double FACT_ATSKL;		// attack skill -> attack bonus
	static const double FACT_DEFSKL;	// defense skill -> defense bonus
	static const double FACT_DEF;		// defense bonus scaling
	static const double FACT_HIT;		// hitbonus scaling
	static const double FACT_WENCH;		// weapon enchantment -> attack bonus
	static const double FACT_AENCH;		// armor enchantment -> defense bonus
	static const double FACT_ATSPD;		// hindrance -> attack speed
	static const double FACT_WALKSPD;	// hindrance -> walk speed

	std::string name;
	uint formatFlags; // combination of FormatFlag
	symbol sym;
	TCODColor color;
	int health, maxHealth;
	int mana, maxMana;
	bool controlled;
	symbol mainWeapon;
	symbol armor[NUM_ARMOR_SLOTS];
	Weapon baseWeapon;
	int baseAC;
	int walkingSpeed;
	int attackSkill;
	int defenseSkill;
	Level* level;
	Point position;
	std::map<symbol,Item*> inventory;
	int lastTimeRegen;
	Point lastPlayerPos;
	bool seenPlayer;

	void die(Creature* instigator);
	Creature(const Creature& copy);

public:

	Creature(); // for savegames
	Creature(std::string name, uint format, symbol sym, TCODColor clr, int maxHealth, int maxMana, Weapon baseWeapon, int baseAC, int walkingSpeed);
	virtual ~Creature();
	virtual Creature* clone();

	std::string getName();
	uint getFormatFlags();
	Point getPos();
	symbol getSymbol();
	TCODColor getColor();
	int getDefense();
	int getAttack();
	bool isControlled();
	Weapon* getMainWeapon();
	std::map<symbol,Item*> getArmor();
	Armor* getArmor(ArmorSlot slot);
	std::map<symbol, Item*> getInventory();
	std::pair<int,int> getHealth();
	std::pair<int,int> getMana();
	int getHindrance();
	int getWalkingSpeed();
	void regenerate(int speedup);

	void setControlled(bool);
	void setLevel(Level* l);
	void setAttackSkill(int attackSkill);
	void setDefenseSkill(int defenseSkill);
	void setBaseWeapon(Weapon base);

	void wieldMainWeapon(Weapon* wpn, int attackSkill);
	void wearArmor(Armor* armor, int defenseSkill);
	void takeOffArmor(Armor* armor);

	bool addItem(Item* item);
	void removeItem(Item* item, bool del);

	int attack(Creature* c);
	void move(Point pos);
	void moveTo(Point pos);

	bool hurt(int damage, Creature* instigator); // TODO : ,DamageType dt

	virtual int action();

	virtual unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

class Goblin: public Creature
{
public:
	Goblin();
	Goblin(std::string name, uint format, symbol sym, TCODColor clr, int maxHealth, int maxMana, Weapon baseWeapon, int baseAC, int walkingSpeed);
	~Goblin();
	Creature* clone();

	int action();
	unsigned int save(Savegame& sg);
};

#endif