#include "../creature.hpp"
#include "../world.hpp"
#include "../player.hpp"
#include "../tileset.hpp"
#include "../level.hpp"
#include "../pathfinding.hpp"
#include <algorithm>
#include <cmath>

Goblin::Goblin()
{
	// empty constructor, for savegames
}

Goblin::Goblin(std::string n, uint f, symbol s, TCODColor c, int h, int m, Weapon w, int a, int ws, int exp):
	Creature(n,f,s,c,h,m,w,a,ws,exp)
{
	// TODO: attackSkill and defenseSkill, for creature too
}

Goblin::~Goblin()
{
	for (std::map<symbol, Item*>::iterator it=inventory.begin(); it!=inventory.end(); it++)
	{
		delete it->second;
	}
}

Creature* Goblin::clone()
{
	Goblin* copy = new Goblin(name, formatFlags, sym, color, maxHealth, maxMana, baseWeapon, baseAC, walkingSpeed, expValue);
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

int Goblin::action()
{
	regenerate(0);

	Point ppos = world.player->getCreature()->getPos();
	// can the goblin see the player?
	bool seesPlayer = true;
	Point p = position;
	TCODLine::init(p.x, p.y, ppos.x, ppos.y);
	do
	{
		if (!world.tileSet->isPassable(level->getTile(p)))
		{
			seesPlayer = false;
			break;
		}
	}
	while (!TCODLine::step(&p.x, &p.y));
	// if player sees me, i see him too
	seesPlayer |= world.fovMap->isInFov(position.x, position.y);
	if (seesPlayer)
	{
		seenPlayer = true;
		lastPlayerPos = ppos;
	}
	else if (seenPlayer)
	{
		if (lastPlayerPos == position)
		{
			seenPlayer = false;
			return 10;
		}
		ppos = lastPlayerPos;
	}
	else
	{
		return 10;
	}

	TCODPath path = TCODPath(level->getWidth(), level->getHeight(), new PathFindingCallback(), level);
	path.compute(position.x, position.y, ppos.x, ppos.y);

	int tx, ty;
	if (!path.isEmpty() && path.walk(&tx, &ty, true))
	{
		Point target = Point(tx, ty);
		Creature* c = level->creatureAt(target);
		if (c != NULL && c->isControlled())
		{
			return attack(c);
		}
		else
		{
			float diagonal = ((target - position).x != 0 && (target - position).y != 0)?(std::sqrt(2.f)):(1.f);
			position = target;
			return static_cast<int>(static_cast<float>(getWalkingSpeed()) * diagonal);
		}
	}
	return walkingSpeed + getHindrance();
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Goblin::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this, &id)) return id;
	SaveBlock store("Goblin", id);
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

// Note: Loading should work fine through Creature::load