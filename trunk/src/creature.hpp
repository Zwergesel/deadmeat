#ifndef _CREATURE_HPP
#define _CREATURE_HPP

#include <libtcod.hpp>
#include <string>
#include <sstream>
#include <map>
#include "utility.hpp"
#include "items/weapon.hpp"
#include "items/armor.hpp"
#include "items/ammo.hpp"

class Level;
class Player;
class Savegame;
class LoadBlock;
class SaveBlock;

// TODO: mark implemented statuses
enum Status
{
  STATUS_WOUND,
  STATUS_POISON,
  STATUS_SICK,
  STATUS_STUN,
  STATUS_PARALYZE,
  STATUS_SLEEP,
  STATUS_FEAR,
  STATUS_CONFUSE,
  STATUS_SLOW,
  STATUS_HASTE,
  STATUS_IMMOBILE,
  STATUS_BEARTRAP,	// immobile, possible to escape
  STATUS_BLIND,
  STATUS_HALLU,
  STATUS_CONDEMN,
  STATUS_FIRE,      // damage over time
  STATUS_FREEZE,
  STATUS_BRAIN,
  STATUS_DRAIN,
  NUM_STATUS
};

enum DamageType
{
  DAMAGE_WEAPON,
  DAMAGE_FIRE,
  DAMAGE_DROWN,
  DAMAGE_SPIKES,
  NUM_DAMAGETYPE
};

struct StatusInfo
{
	Status type;
	int start;
	int duration;
	int strength;
	StatusInfo() {};
	StatusInfo(Status t, int s, int d, int x):type(t),start(s),duration(d),strength(x) {};
};

class Creature
{
protected:

	std::string name;
	uint formatFlags; // combination of FormatFlag
	symbol sym;
	TCODColor color;
	int health, maxHealth;
	int mana, maxMana;
	bool controlled;
	symbol mainWeapon;
	symbol armor[NUM_ARMOR_SLOTS];
	symbol quiver;
	Weapon baseWeapon;
	int baseAC;
	int walkingSpeed;
	int attackSkill;
	int defenseSkill;
	Level* level;
	Point position;
	std::map<symbol,Item*> inventory;
	int expValue;
	int lastTimeRegen;
	int lastTimeManaRegen;
	std::vector<StatusInfo> status;

	void die(Creature* instigator);
	Creature(const Creature& copy);

public:

	Creature(); // for savegames
	Creature(std::string name, uint format, symbol sym, TCODColor clr, int maxHealth, int maxMana, Weapon baseWeapon, int baseAC, int walkingSpeed, int expValue);
	virtual ~Creature();
	virtual Creature* clone();
	void copyFrom(Creature*);

	std::string getName();
	uint getFormatFlags();
	Point getPos();
	void setPos(Point p);
	symbol getSymbol();
	TCODColor getColor();
	int getDefense();
	int getAttack();
	bool isControlled();
	Weapon* getMainWeapon();
	std::map<symbol,Item*> getArmor();
	Armor* getArmor(ArmorSlot slot);
	Ammo* getQuiver();
	std::map<symbol, Item*> getInventory();
	std::pair<int,int> getHealth();
	std::pair<int,int> getMana();
	int getHindrance();
	int getWalkingSpeed();
	void regenerate(int healthSpeedup, int manaSpeedup);

	void setControlled(bool);
	void setLevel(Level* l);
	void setAttackSkill(int attackSkill);
	void setDefenseSkill(int defenseSkill);
	void setBaseWeapon(Weapon base);
	void addMaxHealth(int delta);
	void addMaxMana(int delta);

	virtual void affect(Status type, int start, int duration, int strength);
	void updateStatus(int time);
	int getStatusStrength(Status status);
	void endStatus(Status type);

	void wieldMainWeapon(Weapon* wpn);
	void wearArmor(Armor* armor);
	void takeOffArmor(Armor* armor);
	void readyQuiver(Ammo* ammo);

	symbol expectedInventoryLetter(Item* item);
	Item* addItem(Item* item);
	void removeItem(Item* item, int num, bool del);

	int attack(Point position);
	int rangedAttack(Point position, Weapon* weapon);
	void move(Point pos);
	void moveTo(Point pos);

	bool hurt(int damage, Creature* instigator, DamageType type);
	void kill();

	virtual int action();

	virtual void storeAll(Savegame& sg, SaveBlock& store);
	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);

	static const double FACT_ATSKL;		// attack skill -> attack bonus
	static const double FACT_DEFSKL;	// defense skill -> defense bonus
	static const double FACT_DEF;		// defense bonus scaling
	static const double FACT_HIT;		// hitbonus scaling
	static const double FACT_WENCH;		// weapon enchantment -> attack bonus
	static const double FACT_AENCH;		// armor enchantment -> defense bonus
	static const double FACT_ATSPD;		// hindrance -> attack speed
	static const double FACT_WALKSPD;	// hindrance -> walk speed
};

class BasicMonster : public Creature
{
protected:
	bool defaultUsesMeleeWeapons;
	bool defaultUsesRangedWeapons;
	Point lastSeenPlayer;

	virtual bool usesMeleeWeapons();
	virtual bool usesRangedWeapons();
	virtual void useBestWeapon();
	virtual int scoreWeapon(Weapon*);
	virtual bool shouldFlee();
	virtual bool seePlayer();
	virtual int doRangedAttack(Weapon*);
	virtual int doSpecialAttack();
	virtual int doMeleeAttack(Weapon*);
	virtual int doChargePlayer();
	virtual int doWander();
	int navigateTo(Point);

public:
	BasicMonster();
	BasicMonster(std::string name, uint format, symbol sym, TCODColor clr, int maxHealth, int maxMana, Weapon baseWeapon, int baseAC, int walkingSpeed, int expValue, bool meleeWpn, bool rangedWpn);
	virtual ~BasicMonster();
	virtual Creature* clone();
	void copyFrom(BasicMonster*);

	virtual int action();

	virtual void storeAll(Savegame& sg, SaveBlock& store);
	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);
};

#endif