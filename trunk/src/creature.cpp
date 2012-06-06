#include <sstream>
#include <algorithm>
#include <cassert>
#include "creature.hpp"
#include "player.hpp"
#include "level.hpp"
#include "world.hpp"
#include "items/weapon.hpp"
#include "savegame.hpp"

Creature::Creature()
{
	// for savegames
}

Creature::Creature(Point p, std::string n, symbol s, TCODColor c, int h):
	name(n),
	position(p),
	sym(s),
	color(c),
	health(h),
	maxHealth(h),
	controlled(false),
	mainWeapon(NULL)
{
	baseWeapon = Weapon("hands", (unsigned char)'¤', TCODColor::pink, 10, 10, 10, 0, 0, 0, SKILL_UNARMED, 2);
	baseArmor = Armor("skin", (unsigned char)'¤', TCODColor::pink, 0, 0, ARMOR_BODY, SKILL_UNARMORED);
	attackSkill = 0;
	armorSkill = 0;
	std::fill(armor, armor+NUM_ARMOR_SLOTS, 0);
}

Creature::~Creature()
{
	for (std::map<symbol, Item*>::iterator it=inventory.begin(); it!=inventory.end(); it++)
	{
		delete (*it).second;
	}
}

Creature* Creature::clone()
{
	Creature* copy = new Creature(position, name, sym, color, maxHealth);
	copy->health = health;
	copy->controlled = controlled;
	// TODO: clone weapon and armor, inventory!
	copy->mainWeapon = mainWeapon;
	std::copy(armor, armor+NUM_ARMOR_SLOTS, copy->armor);
	copy->attackSkill = attackSkill;
	copy->armorSkill = armorSkill;
	copy->baseWeapon = baseWeapon;
	copy->armorSkill = armorSkill;
	return copy;
}

std::string Creature::getName()
{
	return name;
}

Point Creature::getPos()
{
	return position;
}

int Creature::getSymbol()
{
	return sym;
}

TCODColor Creature::getColor()
{
	return color;
}

Weapon* Creature::getMainWeapon()
{
	if (inventory.count(mainWeapon) > 0)
	{
		assert(inventory[mainWeapon]->getType() == ITEM_WEAPON);
		return static_cast<Weapon*>(inventory[mainWeapon]);
	}
	return NULL;
}

Armor* Creature::getArmor(ArmorSlot slot)
{
	if (armor[slot] != 0 && inventory.count(armor[slot]) > 0)
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

void Creature::wieldMainWeapon(Weapon* wpn, int skill)
{
	mainWeapon = NULL;
	for (std::map<symbol, Item*>::iterator it=inventory.begin(); it!=inventory.end(); it++)
	{
		if ((*it).second == wpn)
		{
			mainWeapon = (*it).first;
		}
	}
	attackSkill = skill;
}

void Creature::wearArmor(Armor* a, int skill)
{
	armor[a->getSlot()] = 0;
	for (std::map<symbol, Item*>::iterator it=inventory.begin(); it!=inventory.end(); it++)
	{
		if ((*it).second == a)
		{
			armor[a->getSlot()] = (*it).first;
		}
	}
	armorSkill = skill;
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

void Creature::die(Creature* instigator)
{
	if (controlled)
	{
		world.addMessage("You die...", true);
		// This message will not be visible, TODO: think of a better way to do this
		world.addMessage("", true);
		world.gameover = true;
	}
	else
	{
		std::stringstream msg;
		instigator->isControlled() ? (msg << "You kill") : (msg << "The " << instigator->getName() << " kills");
		msg << " the " << name << ".";
		world.addMessage(msg.str());
		level->removeCreature(this, true);
	}
}

int Creature::action()
{
	return 1;
}

int Creature::getAttack()
{
	if (inventory.count(mainWeapon) > 0)
	{
		assert(inventory[mainWeapon]->getType() == ITEM_WEAPON);
		Weapon* w = static_cast<Weapon*>(inventory[mainWeapon]);
		return w->getHitBonus() + w->getEnchantment() + attackSkill;
	}
	else
	{
		return baseWeapon.getHitBonus() + baseWeapon.getEnchantment() + attackSkill;
	}
}

int Creature::getDefense()
{
	// armor + (fighting skill + armor skill)/2
	int defense = armorSkill;
	if (armor[ARMOR_BODY] == 0) defense += baseArmor.getAC();
	for (int slot = 0; slot < NUM_ARMOR_SLOTS; slot++)
	{
		Armor* ar = getArmor(static_cast<ArmorSlot>(slot));
		if (ar != NULL) defense += ar->getAC();
		// TODO: body armor uses it's own skill
	}
	return defense;
}

bool Creature::isControlled()
{
	return controlled;
}

void Creature::setControlled(bool c)
{
	controlled = c;
}

void Creature::setLevel(Level* l)
{
	level = l;
}

int Creature::attack(Creature* target)
{
	// base attack (hands, claws, etc.)
	int attack = baseWeapon.getHitBonus() + baseWeapon.getEnchantment() + attackSkill;
	// base attack damage
	int damage = baseWeapon.rollDamage();
	// base attack speed
	int speed = baseWeapon.getSpeed() - 0;

	if (inventory.count(mainWeapon) > 0)
	{
		assert(inventory[mainWeapon]->getType() == ITEM_WEAPON);
		Weapon* w = static_cast<Weapon*>(inventory[mainWeapon]);
		// (weapon to hit + weapon enchantment) + ((fighting skill + weapon skill)/2)
		attack = w->getHitBonus() + w->getEnchantment() + attackSkill;
		// damage = (weapon damage + weapon enchantment)
		damage = w->rollDamage();
		// weapon speed + armor hindrance
		speed = w->getSpeed() - 0;
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
		controlled ? (msg << "You hit ") : (msg << "The " << name << " hits ");
		target->isControlled() ? (msg << "you for ") : (msg << "the " << target->getName() << " for ");
		msg << damage << " damage.";
		world.addMessage(msg.str());
		target->hurt(damage, this);
	}
	else
	{
		std::stringstream msg;
		controlled ? (msg << "You miss ") : (msg << "The " << name << " misses ");
		target->isControlled() ? (msg << "you.") : (msg << "the " << target->getName() << ".");
		world.addMessage(msg.str());
	}

	return speed;
}

void Creature::setAttackSkill(int attack)
{
	attackSkill = attack;
}

void Creature::setBaseWeapon(Weapon base)
{
	baseWeapon = base;
}

bool Creature::addItem(Item* item)
{
	bool erg = false;
	for (int i=0; i<util::numLetters; i++)
	{
		if (inventory.insert(std::pair<symbol, Item*>(util::letters[i], item)).second)
		{
			erg = true;
			break;
		}
	}
	return erg;
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
	store ("name", name) ("symbol", sym) ("position", position);
	store ("color", color) ("health", health) ("maxHealth", maxHealth);
	store ("controlled", controlled);
	store ("mainWeapon", (int) mainWeapon);
	for (int slot = 0; slot < NUM_ARMOR_SLOTS; slot++)
	{
		store ("armor"+slot, armor[slot]);
	}
	store ("attackSkill", attackSkill) ("armorSkill", armorSkill);
	store.ptr("baseWeapon", baseWeapon.save(sg)).ptr("baseArmor", baseArmor.save(sg));
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
	int intsym;
	load ("name", name) ("symbol", intsym) ("position", position);
	sym = static_cast<symbol>(intsym);
	load ("color", color) ("health", health) ("maxHealth", maxHealth);
	load ("controlled", controlled);
	load ("mainWeapon", intsym);
	mainWeapon = (symbol) intsym;
	for (int slot = 0; slot < NUM_ARMOR_SLOTS; slot++)
	{
		load ("armor"+slot, intsym);
		armor[slot] = (symbol) intsym;
	}
	load ("attackSkill", attackSkill) ("armorSkill", armorSkill);
	baseWeapon = *static_cast<Weapon*>(load.ptr("baseWeapon"));
	baseArmor = *static_cast<Armor*>(load.ptr("baseArmor"));
	int n;
	load ("#inventory", n);
	while (n-->0)
	{
		load ("_symbol", intsym);
		inventory[static_cast<symbol>(intsym)] = static_cast<Item*>(load.ptr("_item"));
	}
}