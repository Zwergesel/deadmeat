#include <sstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "creature.hpp"
#include "player.hpp"
#include "level.hpp"
#include "world.hpp"
#include "items/weapon.hpp"
#include "savegame.hpp"

const double Creature::FACT_ATSKL = 10.0;	// attack skill -> attack bonus
const double Creature::FACT_DEFSKL = 10.0;	// defense skill -> defense bonus
const double Creature::FACT_DEF = 1.0;		// defense bonus scaling
const double Creature::FACT_HIT = 1.0;		// hitbonus scaling
const double Creature::FACT_WENCH = 10.0;	// weapon enchantment -> attack bonus
const double Creature::FACT_AENCH = 10.0;	// armor enchantment -> defense bonus
const double Creature::FACT_ATSPD = 1.0;	// hindrance -> attack speed
const double Creature::FACT_WALKSPD = 1.0; // hindrance -> walk speed

Creature::Creature()
{
	// for savegames, initializes nothing
}

Creature::Creature(std::string n, uint f, symbol s, TCODColor c, int h, int m, Weapon w, int a, int ws, int exp):
	name(n),		formatFlags(f),		sym(s),
	color(c),		health(h),			maxHealth(h),
	mana(m),		maxMana(m),			controlled(false),
	mainWeapon(0),	baseWeapon(w),		baseAC(a),
	walkingSpeed(ws),attackSkill(0),	defenseSkill(0),
	level(NULL),	position(Point(0,0)),
	lastPlayerPos(Point(0,0)), seenPlayer(false), expValue(exp)
{
	std::fill(armor, armor+NUM_ARMOR_SLOTS, 0);
	lastTimeRegen = world.time;
}

Creature::~Creature()
{
	for (std::map<symbol, Item*>::iterator it=inventory.begin(); it!=inventory.end(); it++)
	{
		delete it->second;
	}
}

Creature* Creature::clone()
{
	Creature* copy = new Creature(name, formatFlags, sym, color, maxHealth, maxMana, baseWeapon, baseAC, walkingSpeed, expValue);
	copy->health = health;
	copy->mana = mana;
	copy->controlled = controlled;
	copy->mainWeapon = mainWeapon;
	copy->attackSkill = attackSkill;
	copy->defenseSkill = defenseSkill;
	copy->level = level;
	copy->position = position;
	copy->walkingSpeed = walkingSpeed;
	copy->lastTimeRegen = lastTimeRegen;
	copy->lastPlayerPos = lastPlayerPos;
	copy->seenPlayer = seenPlayer;
	copy->expValue = expValue;
	std::copy(armor, armor+NUM_ARMOR_SLOTS, copy->armor);
	// Clone inventory
	for (std::map<symbol,Item*>::iterator it = inventory.begin(); it != inventory.end(); it++)
	{
		copy->inventory.insert(std::make_pair(it->first, it->second->clone()));
	}
	return copy;
}

std::string Creature::getName()
{
	return name;
}

uint Creature::getFormatFlags()
{
	return formatFlags;
}

Point Creature::getPos()
{
	return position;
}

symbol Creature::getSymbol()
{
	return sym;
}

TCODColor Creature::getColor()
{
	return color;
}

Weapon* Creature::getMainWeapon()
{
	if (mainWeapon > 0 && inventory.count(mainWeapon) > 0)
	{
		assert(inventory[mainWeapon]->getType() == ITEM_WEAPON);
		return static_cast<Weapon*>(inventory[mainWeapon]);
	}
	return NULL;
}

Armor* Creature::getArmor(ArmorSlot slot)
{
	if (armor[slot] > 0 && inventory.count(armor[slot]) > 0)
	{
		assert(inventory[armor[slot]]->getType() == ITEM_ARMOR);
		return static_cast<Armor*>(inventory[armor[slot]]);
	}
	return NULL;
}

std::map<symbol,Item*> Creature::getArmor()
{
	std::map<symbol,Item*> worn;
	for (int slot = 0; slot < NUM_ARMOR_SLOTS; slot++)
	{
		// TODO: this can be optimized for performance
		if (armor[slot] != 0 && inventory.count(armor[slot]) > 0)
		{
			assert(inventory[armor[slot]]->getType() == ITEM_ARMOR);
			worn[armor[slot]] = inventory[armor[slot]];
		}
	}
	return worn;
}

void Creature::wieldMainWeapon(Weapon* wpn)
{
	mainWeapon = 0;
	for (std::map<symbol, Item*>::iterator it=inventory.begin(); it!=inventory.end(); it++)
	{
		if ((*it).second == wpn)
		{
			mainWeapon = (*it).first;
		}
	}
}

void Creature::wearArmor(Armor* a)
{
	armor[a->getSlot()] = 0;
	for (std::map<symbol, Item*>::iterator it=inventory.begin(); it!=inventory.end(); it++)
	{
		if ((*it).second == a)
		{
			armor[a->getSlot()] = (*it).first;
		}
	}
}

void Creature::takeOffArmor(Armor* a)
{
	symbol s = armor[a->getSlot()];
	if (s > 0 && inventory.count(s) > 0 && inventory[s] == a)
	{
		armor[a->getSlot()] = 0;
	}
}

void Creature::move(Point dpos)
{
	position.x += dpos.x;
	position.y += dpos.y;
}

void Creature::moveTo(Point pos)
{
	this->position = pos;
}

std::pair<int,int> Creature::getHealth()
{
	return std::make_pair(health, maxHealth);
}

std::pair<int,int> Creature::getMana()
{
	return std::make_pair(mana, maxMana);
}

bool Creature::hurt(int damage, Creature* instigator)
{
	health -= damage;
	if (health <= 0)
	{
		die(instigator);
		return true;
	}
	return false;
}

void Creature::kill()
{
	die(NULL);
}

void Creature::die(Creature* instigator)
{
	if (controlled)
	{
		world.addMessage("You die...", true);
		// This message will not be visible, but forces confirmation by the player
		world.addMessage("", true);
		world.gameover = true;
	}
	else if (instigator != NULL)
	{
		std::stringstream msg;
		instigator->isControlled() ?
		(msg << "You kill ") :
		(msg << util::format(FORMAT_DEF, instigator->getName(), instigator->getFormatFlags(), true) << " kills");
		msg << util::format(FORMAT_DEF, name, formatFlags) << ".";
		world.addMessage(msg.str());
		if (instigator->isControlled()) world.player->incExperience(expValue);
		level->removeCreature(this, true);
	}
	else
	{
		std::stringstream msg;
		msg << util::format(FORMAT_DEF, name, formatFlags, true) << " dies.";
		world.addMessage(msg.str());
		level->removeCreature(this, true);
	}
}

void Creature::regenerate(int speedup)
{
	if ((world.time - lastTimeRegen) > (70 - speedup))
	{
		health += (world.time - lastTimeRegen) / (70 - speedup);
		health = std::min(maxHealth, health);
		lastTimeRegen = world.time;
	}
}

int Creature::action()
{
	return 1;
}

int Creature::getAttack()
{
	if (mainWeapon > 0 && inventory.count(mainWeapon) > 0)
	{
		assert(inventory[mainWeapon]->getType() == ITEM_WEAPON);
		Weapon* w = static_cast<Weapon*>(inventory[mainWeapon]);
		return static_cast<int>(FACT_HIT * w->getHitBonus() + FACT_WENCH * w->getEnchantment() + FACT_ATSKL * attackSkill);
	}
	else
	{
		return static_cast<int>(FACT_HIT * baseWeapon.getHitBonus() + FACT_WENCH * baseWeapon.getEnchantment() + FACT_ATSKL * attackSkill);
	}
}

int Creature::getDefense()
{
	double defense = FACT_DEFSKL * defenseSkill;
	if (armor[ARMOR_BODY] == 0) defense += FACT_DEF * baseAC;
	for (int slot = 0; slot < NUM_ARMOR_SLOTS; slot++)
	{
		Armor* ar = getArmor(static_cast<ArmorSlot>(slot));
		if (ar != NULL) defense += FACT_DEF * ar->getDefense() + FACT_AENCH * ar->getEnchantment();
	}
	return static_cast<int>(defense);
}

int Creature::getHindrance()
{
	int hindrance = 0;
	for (int slot = 0; slot < NUM_ARMOR_SLOTS; slot++)
	{
		Armor* ar = getArmor(static_cast<ArmorSlot>(slot));
		if (ar != NULL) hindrance += ar->getHindrance();
	}
	return hindrance;
}

int Creature::getWalkingSpeed()
{
	return static_cast<int>(walkingSpeed + FACT_WALKSPD * getHindrance());
}

bool Creature::isControlled()
{
	return controlled;
}

void Creature::setControlled(bool c)
{
	controlled = c;
}

void Creature::setPos(Point p)
{
	position = p;
}

void Creature::setLevel(Level* l)
{
	level = l;
}

void Creature::addMaxHealth(int delta)
{
	maxHealth += delta;
	if (maxHealth < 1)
	{
		if (controlled) world.addMessage("You feel your remaining life force getting sucked out of you.");
		die(NULL);
	}
}

int Creature::attack(Creature* target)
{
	// base attack (hands, claws, etc.)
	int attack = static_cast<int>(FACT_HIT * baseWeapon.getHitBonus() + FACT_WENCH * baseWeapon.getEnchantment() + FACT_ATSKL * attackSkill);
	// base attack damage
	int damage = baseWeapon.rollDamage();
	// base attack speed
	int speed = static_cast<int>(baseWeapon.getSpeed() + FACT_ATSPD * getHindrance());

	if (mainWeapon > 0 && inventory.count(mainWeapon) > 0)
	{
		assert(inventory[mainWeapon]->getType() == ITEM_WEAPON);
		Weapon* w = static_cast<Weapon*>(inventory[mainWeapon]);
		// Ranged weapons are not used in close combat
		if (w->getRange() <= 1)
		{
			// weapon to hit + weapon enchantment + fighting skill + weapon skill
			attack = static_cast<int>(FACT_HIT * w->getHitBonus() + FACT_WENCH * w->getEnchantment() + FACT_ATSKL * attackSkill);
			damage = w->rollDamage();
			// weapon speed + armor hindrance
			speed = static_cast<int>(w->getSpeed() + FACT_ATSPD * getHindrance());
		}
	}

	// skill boni
	if (controlled)
	{
		speed = std::max(1, static_cast<int>(speed * world.player->getWeaponSpeedBonus()));
		damage += world.player->rollBonusDamage();
	}

	int defense = target->getDefense();
	TCODRandom rngGauss;
	rngGauss.setDistribution(TCOD_DISTRIBUTION_GAUSSIAN_RANGE);
	int mean = attack - defense;
	int hit = 0;
	if (mean >= 0) hit = rngGauss.getInt(-300, 300, mean);
	if (mean < 0) hit = -rngGauss.getInt(-300, 300, -mean);
	if (hit >= -70)
	{
		if (hit <= 0) damage /= 2;
		if (hit > 175) damage *= 2;
		std::stringstream msg;
		controlled ? (msg << "You hit ") :
		(msg << util::format(FORMAT_DEF, name, formatFlags, true) << " hits ");
		target->isControlled() ? (msg << "you for ") :
		(msg << util::format(FORMAT_DEF, target->getName(), target->getFormatFlags()) << " for ");
		msg << damage << " damage.";
		world.addMessage(msg.str());
		target->hurt(damage, this);
	}
	else
	{
		std::stringstream msg;
		controlled ? (msg << "You miss ") :
		(msg << util::format(FORMAT_DEF, name, formatFlags, true) << " misses ");
		target->isControlled() ? (msg << "you.") :
		(msg << util::format(FORMAT_DEF, target->getName(), target->getFormatFlags()) << ".");
		world.addMessage(msg.str());
	}

	return speed;
}

int Creature::rangedAttack(Creature* target, Weapon* w)
{
	assert(w->getType() == ITEM_WEAPON);
	assert(w->getRange() > 1);
	// weapon to hit + weapon enchantment + fighting skill + weapon skill
	int attack = static_cast<int>(FACT_HIT * w->getHitBonus() + FACT_WENCH * w->getEnchantment() + FACT_ATSKL * attackSkill);
	int damage = w->rollDamage();
	// weapon speed + armor hindrance
	int speed = static_cast<int>(w->getSpeed() + FACT_ATSPD * getHindrance());
	// TODO: scale attack difficulty with distance

	int defense = target->getDefense();
	TCODRandom rngGauss;
	rngGauss.setDistribution(TCOD_DISTRIBUTION_GAUSSIAN_RANGE);
	int mean = attack - defense;
	int hit = 0;
	if (mean >= 0) hit = rngGauss.getInt(-300, 300, mean);
	if (mean < 0) hit = -rngGauss.getInt(-300, 300, -mean);
	if (hit >= -70)
	{
		if (hit <= 0) damage /= 2;
		if (hit > 175) damage *= 2;
		std::stringstream msg;
		controlled ? (msg << "You shoot ") :
		(msg << util::format(FORMAT_DEF, name, formatFlags, true) << " shoots ");
		target->isControlled() ? (msg << "you for ") :
		(msg << util::format(FORMAT_DEF, target->getName(), target->getFormatFlags()) << " for ");
		msg << damage << " damage.";
		world.addMessage(msg.str());
		target->hurt(damage, this);
	}
	else
	{
		std::stringstream msg;
		controlled ? (msg << "You miss ") :
		(msg << util::format(FORMAT_DEF, name, formatFlags, true) << " misses ");
		target->isControlled() ? (msg << "you.") :
		(msg << util::format(FORMAT_DEF, target->getName(), target->getFormatFlags()) << ".");
		world.addMessage(msg.str());
	}

	return speed;
}

void Creature::setAttackSkill(int value)
{
	attackSkill = value;
}

void Creature::setDefenseSkill(int value)
{
	defenseSkill = value;
}

void Creature::setBaseWeapon(Weapon base)
{
	baseWeapon = base;
}

symbol Creature::addItem(Item* item)
{
	for (int i=0; i<util::numLetters; i++)
	{
		if (inventory.insert(std::pair<symbol, Item*>(util::letters[i], item)).second)
		{
			return util::letters[i];
		}
	}
	return 0;
}

void Creature::removeItem(Item* item, bool del)
{
	for (std::map<symbol, Item*>::iterator it=inventory.begin(); it!=inventory.end(); it++)
	{
		if (item == (*it).second)
		{
			if (del && (*it).second != NULL)
			{
				delete (*it).second;
				(*it).second = NULL;
			}
			inventory.erase(it);
			break;
		}
	}
}

std::map<symbol, Item*> Creature::getInventory()
{
	return inventory;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Creature::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this, &id)) return id;
	SaveBlock store("Creature", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("position", position);
	store ("color", color) ("health", health) ("maxHealth", maxHealth);
	store ("controlled", controlled);
	store ("mainWeapon", (int) mainWeapon);
	for (int slot = 0; slot < NUM_ARMOR_SLOTS; slot++)
	{
		std::stringstream ss;
		ss << "armor" << slot;
		store (ss.str(), armor[slot]);
	}
	store.ptr("baseWeapon", baseWeapon.save(sg));
	store ("baseAC", baseAC) ("attackSkill", attackSkill) ("defenseSkill", defenseSkill);
	store ("walkingSpeed", walkingSpeed);
	store ("lastTimeRegen", lastTimeRegen);
	store ("lastPlayerPos", lastPlayerPos) ("seenPlayer", seenPlayer);
	store ("expValue", expValue);
	store ("#inventory", (int) inventory.size());
	for (std::map<symbol, Item*>::iterator it = inventory.begin(); it != inventory.end(); it++)
	{
		store ("_symbol", (int) it->first) .ptr("_item", it->second->save(sg));
	}
	sg << store;
	return id;
}

void Creature::load(LoadBlock& load)
{
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("position", position);
	load ("color", color) ("health", health) ("maxHealth", maxHealth);
	load ("controlled", controlled);
	load ("mainWeapon", mainWeapon);
	for (int slot = 0; slot < NUM_ARMOR_SLOTS; slot++)
	{
		std::stringstream ss;
		ss << "armor" << slot;
		load (ss.str(), armor[slot]);
	}
	baseWeapon = *static_cast<Weapon*>(load.ptr("baseWeapon"));
	load ("baseAC", baseAC) ("attackSkill", attackSkill) ("defenseSkill", defenseSkill);
	load ("walkingSpeed", walkingSpeed);
	load ("lastTimeRegen", lastTimeRegen);
	load ("lastPlayerPos", lastPlayerPos) ("seenPlayer", seenPlayer);
	load ("expValue", expValue);
	int n;
	load ("#inventory", n);
	while (n-->0)
	{
		symbol s;
		load ("_symbol", s);
		inventory[s] = static_cast<Item*>(load.ptr("_item"));
	}
}