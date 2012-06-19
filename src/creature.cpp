#include <sstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "creature.hpp"
#include "player.hpp"
#include "level.hpp"
#include "world.hpp"
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
	mainWeapon(0), quiver(0),	baseWeapon(w),		baseAC(a),
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
	copy->quiver = quiver;
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
			it->second->setActive(true);
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
			it->second->setActive(true);
			armor[a->getSlot()] = (*it).first;
		}
	}
}

void Creature::takeOffArmor(Armor* a)
{
	symbol s = armor[a->getSlot()];
	if (s > 0 && inventory.count(s) > 0 && inventory[s] == a)
	{
		a->setActive(false);
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
		(msg << util::format(FORMAT_DEF, instigator, true) << " kills");
		msg << util::format(FORMAT_DEF, this) << ".";
		world.addMessage(msg.str());
		// Experience for player
		if (instigator->isControlled()) world.player->incExperience(expValue);
	}
	else
	{
		std::stringstream msg;
		msg << util::format(FORMAT_DEF, this, true) << " dies.";
		world.addMessage(msg.str());
	}

	if (!controlled)
	{
		// Drop all items and remove creature from level
		for (auto it=inventory.begin(); it!=inventory.end(); it++)
		{
			level->addItem(it->second, position);
		}
		inventory.clear();
		level->removeCreature(this, true);
	}
}

void Creature::regenerate(int speedup)
{
	if ((world.time - lastTimeRegen) > (30 - speedup))
	{
		int bonus = (world.time - lastTimeRegen) / (30 - speedup);
		health = std::min(maxHealth, health + bonus);
		lastTimeRegen += (30 - speedup) * bonus;
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
	return controlled ? static_cast<int>(world.player->getArmorHindranceReduction() * hindrance) : hindrance;
}

int Creature::getWalkingSpeed()
{
	float reduction = controlled ? world.player->getMoveSpeedBonus() : 1.0f;
	return std::max(1, static_cast<int>((walkingSpeed + FACT_WALKSPD * getHindrance()) * reduction));
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
	maxHealth = std::min(maxHealth + delta, 9999);
	if (maxHealth < 1)
	{
		if (controlled) world.addMessage("You feel your remaining life force getting sucked out of you.");
		die(NULL);
	}
}

void Creature::addMaxMana(int delta)
{
	maxMana = util::clamp(maxMana + delta, 0, 9999);
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
		(msg << util::format(FORMAT_DEF, this, true) << " hits ");
		target->isControlled() ? (msg << "you for ") :
		(msg << util::format(FORMAT_DEF, target) << " for ");
		msg << damage << " damage.";
		world.addMessage(msg.str());
		target->hurt(damage, this);
	}
	else
	{
		std::stringstream msg;
		controlled ? (msg << "You miss ") :
		(msg << util::format(FORMAT_DEF, this, true) << " misses ");
		target->isControlled() ? (msg << "you.") :
		(msg << util::format(FORMAT_DEF, target) << ".");
		world.addMessage(msg.str());
	}

	return speed;
}

int Creature::rangedAttack(Creature* target, Weapon* w)
{
	Ammo* ammo = getQuiver();
	assert(ammo != NULL);
	assert(w->getType() == ITEM_WEAPON);
	assert(w->getRange() > 1);
	// weapon to hit + weapon enchantment + fighting skill + weapon skill
	int attack = static_cast<int>(FACT_HIT * w->getHitBonus() + FACT_WENCH * (w->getEnchantment() + ammo->getEnchantment()) + FACT_ATSKL * attackSkill);
	int damage = w->rollDamage();
	// weapon speed + armor hindrance
	int speed = static_cast<int>(w->getSpeed() + FACT_ATSPD * getHindrance());
	// TODO: scale attack difficulty with distance

	// Make attack roll
	int defense = target->getDefense();
	TCODRandom rngGauss;
	rngGauss.setDistribution(TCOD_DISTRIBUTION_GAUSSIAN_RANGE);
	int mean = attack - defense;
	int hit = 0;
	if (mean >= 0) hit = rngGauss.getInt(-300, 300, mean);
	if (mean < 0) hit = -rngGauss.getInt(-300, 300, -mean);

	// Traverse the line of the shot
	TCODRandom* rng = TCODRandom::getInstance();
	Point destination = target->getPos();
	TCODLine flightpath;
	flightpath.init(position.x, position.y, destination.x, destination.y);
	Point current;
	while (!flightpath.step(&current.x, &current.y))
	{
		if (world.tileSet->isBlocking(level->getTile(current)))
		{
			std::stringstream msg;
			msg << util::format(FORMAT_YOUR, ammo->getName(), ammo->getFormatFlags(), true) << " hits ";
			msg << world.tileSet->getDescription(level->getTile(current)) << ".";
			world.addMessage(msg.str());
			return speed;
		}
		Creature* between = level->creatureAt(current);
		if (between != NULL && rng->getInt(-150,300) > std::abs(hit)) // TODO: find a good value here
		{
			target = between;
			break;
		}
	}
	
	// Remove ammo
	if (ammo->getAmount() == 1) readyQuiver(NULL);
	removeItem(ammo, 1, true);

	if (hit >= -70)
	{
		if (hit <= 0) damage /= 2;
		if (hit > 175) damage *= 2;
		std::stringstream msg;
		controlled ? (msg << "You shoot ") :
		(msg << util::format(FORMAT_DEF, this, true) << " shoots ");
		target->isControlled() ? (msg << "you for ") :
		(msg << util::format(FORMAT_DEF, target) << " for ");
		msg << damage << " damage.";
		world.addMessage(msg.str());
		target->hurt(damage, this);
	}
	else
	{
		std::stringstream msg;
		controlled ? (msg << "You miss ") :
		(msg << util::format(FORMAT_DEF, this, true) << " misses ");
		target->isControlled() ? (msg << "you.") :
		(msg << util::format(FORMAT_DEF, target) << ".");
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

symbol Creature::expectedInventoryLetter(Item* item)
{
	for (auto it = inventory.begin(); it != inventory.end(); it++)
	{
		if (item->canStackWith(it->second)) return it->first;
	}
	if ((int)inventory.size() == util::numLetters) return 0;
	for (int i=0; i<util::numLetters; i++)
	{
		if (inventory.find(util::letters[i]) == inventory.end()) return util::letters[i];
	}
	assert(false);
	return 0;
}

Item* Creature::addItem(Item* item)
{
	// Try stacking the item
	for (auto it = inventory.begin(); it != inventory.end(); it++)
	{
		if (item->canStackWith(it->second))
		{
			assert(item != it->second);
			it->second->changeAmount(item->getAmount());
			delete item;
			return it->second;
		}
	}
	// Insert the item in a new slot
	for (int i=0; i<util::numLetters; i++)
	{
		auto it = inventory.find(util::letters[i]);
		if (it == inventory.end())
		{
			std::pair<symbol,Item*> in(util::letters[i], item);
			inventory.insert(it, in);
			return item;
		}
	}
	// It is the developer's responsibility to make sure there's room to insert
	assert(false);
	return item;
}

void Creature::removeItem(Item* item, int num, bool del)
{
	for (auto it = inventory.begin(); it != inventory.end(); it++)
	{
		if (item == it->second)
		{
			assert(item->getAmount() >= num);
			if (item->getAmount() == num)
			{
				if (del) delete item;
				inventory.erase(it);
			}
			else if (!del)
			{
				item->changeAmount(-num);
				it->second = item->clone();
				item->setAmount(num);
			}
			else
			{
				item->changeAmount(-num);
			}
			break;
		}
	}
}

std::map<symbol, Item*> Creature::getInventory()
{
	return inventory;
}

Ammo* Creature::getQuiver()
{
	if (quiver > 0 && inventory.count(quiver) > 0)
	{
		assert(inventory[quiver]->getType() == ITEM_AMMO);
		return static_cast<Ammo*>(inventory[quiver]);
	}
	return NULL;
}

void Creature::readyQuiver(Ammo* ammo)
{
	quiver = 0;
	if (ammo == NULL) return;
	for (auto it=inventory.begin(); it!=inventory.end(); it++)
	{
		if ((*it).second == ammo)
		{
			it->second->setActive(true);
			quiver = (*it).first;
			return;
		}
	}
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Creature::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this, &id)) return id;
	SaveBlock store("Creature", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("position", position);
	store ("color", color) ("health", health) ("maxHealth", maxHealth);
	store ("mana", mana) ("maxMana", maxMana) ("controlled", controlled);
	store ("quiver", (int) quiver);
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
	load ("mana", mana) ("maxMana", maxMana) ("controlled", controlled);
	load ("quiver", quiver);
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