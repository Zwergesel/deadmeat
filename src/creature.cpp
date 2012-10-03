#include <sstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "creature.hpp"
#include "player.hpp"
#include "level.hpp"
#include "world.hpp"
#include "savegame.hpp"
#include "items/corpse.hpp"

const double Creature::FACT_ATSKL = 10.0;	// attack skill -> attack bonus
const double Creature::FACT_DEFSKL = 10.0;	// defense skill -> defense bonus
const double Creature::FACT_DEF = 1.0;		// defense bonus scaling
const double Creature::FACT_HIT = 1.0;		// hitbonus scaling
const double Creature::FACT_WENCH = 10.0;	// weapon enchantment -> attack bonus
const double Creature::FACT_AENCH = 10.0;	// armor enchantment -> defense bonus
const double Creature::FACT_ATSPD = 1.0;	// hindrance -> attack speed
const double Creature::FACT_WALKSPD = 1.0;  // hindrance -> walk speed

Creature::Creature()
{
	// for savegames, initializes nothing
}

Creature::Creature(std::string n, uint f, symbol s, TCODColor c, int h, int m, Weapon w, int a, int ws, int exp, const std::string& corp):
	name(n),		formatFlags(f),		sym(s),
	color(c),		health(h),			maxHealth(h),
	mana(m),		maxMana(m),			controlled(false),
	mainWeapon(0), quiver(0),	baseWeapon(w),		baseAC(a),
	walkingSpeed(ws),attackSkill(0),	defenseSkill(0),
	level(NULL),	position(Point(0,0)), expValue(exp),
	lastTimeRegen(world.time), lastTimeManaRegen(world.time),
	corpseName(corp)
{
	std::fill(armor, armor+NUM_ARMOR_SLOTS, 0);
	std::fill(spells, spells+NUM_SPELL, false);
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
	Creature* copy = new Creature();
	copy->copyFrom(this);
	return copy;
}

void Creature::copyFrom(Creature* original)
{
	name = original->name;
	formatFlags = original->formatFlags;
	sym = original->sym;
	color = original->color;
	maxHealth = original->maxHealth;
	maxMana = original->maxMana;
	baseWeapon = original->baseWeapon;
	baseAC = original->baseAC;
	walkingSpeed = original->walkingSpeed;
	expValue = original->expValue;
	health = original->health;
	mana = original->mana;
	controlled = original->controlled;
	mainWeapon = original->mainWeapon;
	quiver = original->quiver;
	attackSkill = original->attackSkill;
	defenseSkill = original->defenseSkill;
	level = original->level;
	position = original->position;
	walkingSpeed = original->walkingSpeed;
	lastTimeRegen = original->lastTimeRegen;
	lastTimeManaRegen = original->lastTimeManaRegen;
	expValue = original->expValue;
	corpseName = original->corpseName;
	std::copy(original->armor, original->armor+NUM_ARMOR_SLOTS, armor);
	std::copy(original->spells, original->spells+NUM_SPELL, spells);
	// Clone inventory
	for (std::map<symbol,Item*>::iterator it = original->inventory.begin(); it != original->inventory.end(); it++)
	{
		inventory.insert(std::make_pair(it->first, it->second->clone()));
	}
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

bool Creature::knowsSpell(SPELL spell)
{
	return spells[spell];
}

void Creature::learnSpell(SPELL spell)
{
	spells[spell] = true;
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

void Creature::moveTo(Point pos)
{
	this->position = pos;
	assert(level != NULL);
	Object* obj = level->objectAt(pos);
	if (obj != NULL) obj->onStep(this);
}

std::pair<int,int> Creature::getHealth()
{
	return std::make_pair(health, maxHealth);
}

std::pair<int,int> Creature::getMana()
{
	return std::make_pair(mana, maxMana);
}

bool Creature::hurt(int damage, Creature* instigator, DamageType type)
{
	health -= damage;
	if (health <= 0)
	{
		die(instigator);
		return true;
	}
	return false;
}

void Creature::heal(int amount)
{
	health = std::min(health + amount, maxHealth);
}

void Creature::kill()
{
	die(NULL);
}

void Creature::addMana(int amount)
{
	mana = util::clamp(mana + amount, 0, maxMana);
}

void Creature::die(Creature* instigator)
{
	if (controlled)
	{
		if (instigator != NULL)
		{
			std::stringstream msg;
			msg << "You were killed by " << util::format(FORMAT_INDEF, instigator) << ".";
			world.deathReason = msg.str();
		}
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
		// Drop all items
		for (auto it=inventory.begin(); it!=inventory.end(); it++)
		{
			level->addItem(it->second, position);
		}
		inventory.clear();
		// Drop a corpse
		if (corpseName.length() > 0)
		{
			Item* item = factory.spawnItem(corpseName, true);
			assert(item->getType() == ITEM_CORPSE);
			Corpse* corpse = static_cast<Corpse*>(item);
			corpse->initRotTime();
			level->addItem(corpse, position);
		}
		// Remove creature from level
		level->removeCreature(this, false);
		world.garbage.push_back(this);
	}
}

void Creature::regenerate(int healthSpeedup, int manaSpeedup)
{
	if ((world.time - lastTimeRegen) > (30 - healthSpeedup))
	{
		int bonus = (world.time - lastTimeRegen) / (30 - healthSpeedup);
		health = std::min(maxHealth, health + bonus);
		// Don't just set lastTimeRegen to world.time, because you are losing time that did not divide evenly that way
		lastTimeRegen += (30 - healthSpeedup) * bonus;
	}
	if ((world.time - lastTimeManaRegen) > (30 - manaSpeedup))
	{
		int bonus = (world.time - lastTimeManaRegen) / (30 - manaSpeedup);
		mana = std::min(maxMana, mana + bonus);
		lastTimeManaRegen += (30 - manaSpeedup) * bonus;
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
	int slow = getStatusStrength(STATUS_SLOW);
	if (controlled) slow = std::max(0, slow - world.player->getSkill(SKILL_NEG_EFFECT).value);
	int haste = std::max(0, getStatusStrength(STATUS_HASTE));
	return std::max(1, static_cast<int>((walkingSpeed + FACT_WALKSPD * getHindrance()) * reduction + slow - haste));
}

bool Creature::isControlled()
{
	return controlled;
}

void Creature::setControlled(bool c)
{
	controlled = c;
}

void Creature::setPos(Point pos)
{
	position = pos;
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
	else if (health > maxHealth)
	{
		health = maxHealth;
	}
}

void Creature::addMaxMana(int delta)
{
	maxMana = util::clamp(maxMana + delta, 0, 9999);
}

int Creature::attack(Point target)
{
	// baseWeapon or wielded melee weapon, but not ranged weapons
	Weapon* weapon = &baseWeapon;
	Weapon* main = (mainWeapon > 0 ? static_cast<Weapon*>(inventory[mainWeapon]) : NULL);
	assert(main == NULL || main->getType() == ITEM_WEAPON);
	if (main != NULL && main->getRange() <= 1) weapon = main;

	// defense gets calculated later
	int defense = 0;
	int attack = static_cast<int>(FACT_HIT * weapon->getHitBonus() + FACT_WENCH * weapon->getEnchantment() + FACT_ATSKL * attackSkill);
	int damage = weapon->rollDamage();
	int speed = static_cast<int>(weapon->getSpeed() + FACT_ATSPD * getHindrance());

	// skill bonus
	if (controlled)
	{
		speed = std::max(1, static_cast<int>(speed * world.player->getWeaponSpeedBonus()));
		damage += world.player->rollBonusDamage();
	}

	// Attack creatures
	Creature* victim = level->creatureAt(target);
	if (victim != NULL)
	{
		defense = victim->getDefense();
		TCODRandom rngGauss;
		rngGauss.setDistribution(TCOD_DISTRIBUTION_GAUSSIAN_RANGE);
		int mean = util::clamp(1000 + attack - defense, 500, 1500);
		int hit = rngGauss.getInt(700, 1300, mean);
		if (hit >= 930)
		{
			// Half damage or critical damage
			if (hit <= 1000) damage /= 2;
			if (hit > 1175) damage *= 2;

			// Message about hit
			std::stringstream msg;
			controlled ? (msg << "You hit ") :
			(msg << util::format(FORMAT_DEF, this, true) << " hits ");
			victim->isControlled() ? (msg << "you for ") :
			(msg << util::format(FORMAT_DEF, victim) << " for ");
			msg << damage << " damage.";
			world.addMessage(msg.str());

			// Apply pre weapon effects
			WeaponEffect effect = weapon->getEffect();
			DamageType dmgtype = DAMAGE_WEAPON;

			// Hurt the target
			victim->hurt(damage, this, dmgtype);

			// Apply post weapon effects
			if (effect == EFFECT_POISON)
			{
				victim->affect(STATUS_POISON, 0, 200, 10);
			}
			else if (effect == EFFECT_FIRE)
			{
				victim->affect(STATUS_FIRE, 0, 100, 5);
			}
			else if (effect == EFFECT_KNOCKBACK)
			{
				Point knock = target + target - position;
				if (level->creatureAt(knock) == NULL && !level->isBlocking(knock) && rng->getFloat(0.0,1.0) > 0.5)
				{
					victim->moveTo(knock);
					std::stringstream msg;
					controlled ? msg << "Your strike pushes " : msg << util::format(FORMAT_DEF, this, true) << "'s strike pushes ";
					controlled ? msg << util::format(FORMAT_DEF, this) << " back." : msg << "you back.";
					world.addMessage(msg.str());
				}
			}
		}
		else
		{
			std::stringstream msg;
			controlled ? (msg << "You miss ") :
			(msg << util::format(FORMAT_DEF, this, true) << " misses ");
			victim->isControlled() ? (msg << "you.") :
			(msg << util::format(FORMAT_DEF, victim) << ".");
			world.addMessage(msg.str());
		}
		return speed;
	}

	// Hit objects
	Object* obj = level->objectAt(target);
	if (obj != NULL && obj->onAttack(this, attack, damage, weapon))
	{
		// Note: obj->onAttack should handle all effects if it returns true
		return speed;
	}
	// Hit world
	else if (world.tileSet->isBlocking(level->getTile(target)))
	{
		// TODO: Messages for non-controlled creatures
		std::stringstream msg;
		msg << "You bash " << util::format(FORMAT_YOUR, weapon->getName(), weapon->getFormatFlags());
		msg << " against " << world.tileSet->getDescription(level->getTile(target)) << ".";
		world.addMessage(msg.str());

		int broken = rng->getInt(0,999);
		broken = (broken / 800) + (broken / 950) + (broken / 995);
		if (weapon->breakWeapon(broken)) world.addMessage("You hear a cracking sound.");
	}
	// Hit nothing
	else
	{
		// TODO: Message for non-controlled creatures
		world.addMessage("You strike at thin air.");
	}

	return speed;
}

int Creature::rangedAttack(Point position, Weapon* w)
{
	Creature* target = level->creatureAt(position);
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
	Point destination = target->getPos();
	TCODLine::init(position.x, position.y, destination.x, destination.y);
	Point current;
	while (!TCODLine::step(&current.x, &current.y))
	{
		// Hit objects or terrain tiles
		if (level->isBlocking(current))
		{
			std::stringstream msg;
			Object* obj = level->objectAt(current);
			msg << util::format(FORMAT_YOUR, ammo->getName(), ammo->getFormatFlags(), true) << " hits ";
			msg << (obj != NULL ? util::format(FORMAT_INDEF, obj) : world.tileSet->getDescription(level->getTile(current))) << ".";
			world.addMessage(msg.str());
			return speed;
		}
		// Hit other creatures
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
		target->hurt(damage, this, DAMAGE_WEAPON);
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
	if (item->getType() == ITEM_GOLD) return '$';
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
	// Special letter for gold
	if (item->getType() == ITEM_GOLD)
	{
		assert(inventory.find('$') == inventory.end());
		inventory['$'] = item;
		return item;
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

std::map<symbol, Item*>& Creature::getInventory()
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

// Usage: Process immunities or strength reductions in overloaded method, then call this method
void Creature::affect(Status type, int start, int duration, int strength)
{
	int memory = 0;
	if (type == STATUS_POISON)
	{
		strength = std::min(strength, maxHealth-1);
	}

	// Stack with existing effect
	for (auto it = status.begin(); it != status.end(); it++)
	{
		if (it->type == type)
		{
			// TODO: decide via type how stacking works
			it->strength = std::max(it->strength, strength);
			it->duration = std::max(it->duration, (duration - start));
			return;
		}
	}
	status.push_back(StatusInfo(type, start, duration, strength, memory));
}

void Creature::updateStatus(int time)
{
	for (int d=status.size() - 1; d >= 0; d--)
	{
		StatusInfo& stat = status[d];
		if (stat.start >= time)
		{
			stat.start -= time;
			continue;
		}
		time -= stat.start;
		stat.start = 0;
		// Effect
		switch (stat.type)
		{
		case STATUS_FIRE:
			if (controlled)
			{
				world.addMessage("You are burning!");
				world.deathReason = "You burnt to death.";
			}
			hurt(static_cast<int>(time*stat.strength/10.0f), NULL, DAMAGE_FIRE);
			break;
		case STATUS_POISON:
			if (stat.memory < stat.strength)
			{
				int loss = std::min(stat.strength - stat.memory, static_cast<int>(time/5.f));
				maxHealth -= loss;
				health = std::min(health, maxHealth);
				stat.memory += loss;
				std::cerr << "Poison took " << loss << " hp, max " << stat.strength << " current " << stat.memory << std::endl;
			}
			break;
		default:
			std::cerr << name << " has effect " << stat.type << " strength " << stat.strength << " for " << stat.duration << std::endl;
		}
		// Reduce duration
		stat.duration -= time;
		if (stat.duration <= 0) endStatus(stat.type);
	}
}

void Creature::endStatus(Status type)
{
	for (auto it = status.begin(); it != status.end(); it++)
	{
		if (it->type != type) continue;
		switch (type)
		{
		case STATUS_FIRE:
			if (controlled) world.addMessage("The flames go out.");
			break;
		case STATUS_POISON:
			maxHealth += it->memory;
			break;
		default:
			std::cerr << name << " ends effect " << type << std::endl;
		}
		status.erase(it);
		break;
	}
}

int Creature::getStatusStrength(Status type)
{
	for (auto it = status.begin(); it != status.end(); it++)
	{
		if (it->type == type) return it->start == 0 ? it->strength : 0;
	}
	return 0;
}

/*--------------------- SAVING AND LOADING ---------------------*/

void Creature::storeAll(Savegame& sg, SaveBlock& store)
{
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
	store ("lastTimeRegen", lastTimeRegen) ("lastTimeManaRegen", lastTimeManaRegen);
	store ("expValue", expValue) ("corpseName", corpseName);
	store ("#inventory", (int) inventory.size());
	for (std::map<symbol, Item*>::iterator it = inventory.begin(); it != inventory.end(); it++)
	{
		store ("_symbol", (int) it->first) .ptr("_item", it->second->save(sg));
	}
	int numspells = 0;
	for (int spell = 0; spell < NUM_SPELL; spell++)
	{
		if (spells[spell]) numspells++;
	}
	store ("#spells", numspells);
	for (int spell = 0; spell < NUM_SPELL; spell++)
	{
		if (spells[spell]) store ("_spell", spell);
	}
	store ("#status", (int) status.size());
	for (auto it = status.begin(); it != status.end(); it++)
	{
		store ("_type", (int) it->type) ("_start", it->start) ("_duration", it->duration);
		store ("_strength", it->strength) ("_memory", it->memory);
	}
}

unsigned int Creature::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this, &id)) return id;
	SaveBlock store("Creature", id);
	storeAll(sg, store);
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
	Weapon* w = static_cast<Weapon*>(load.ptr("baseWeapon"));
	baseWeapon = *w;
	delete w;
	load ("baseAC", baseAC) ("attackSkill", attackSkill) ("defenseSkill", defenseSkill);
	load ("walkingSpeed", walkingSpeed);
	load ("lastTimeRegen", lastTimeRegen) ("lastTimeManaRegen", lastTimeManaRegen);
	load ("expValue", expValue) ("corpseName", corpseName);
	int n;
	load ("#inventory", n);
	while (n-->0)
	{
		symbol s;
		load ("_symbol", s);
		inventory[s] = static_cast<Item*>(load.ptr("_item"));
	}
	load ("#spells", n);
	std::fill(spells, spells+NUM_SPELL, false);
	while (n-->0)
	{
		int index;
		load ("_spell", index);
		spells[index] = true;
	}
	load ("#status", n);
	while (n-->0)
	{
		int t;
		load ("_type", t);
		if (t < 0 || t >= NUM_STATUS) throw SavegameFormatException("Creature::load _ status out of bounds");
		StatusInfo stat;
		stat.type = static_cast<Status>(t);
		load ("_start", stat.start) ("_duration", stat.duration) ("_strength", stat.strength) ("_memory", stat.memory);
		status.push_back(stat);
	}
}