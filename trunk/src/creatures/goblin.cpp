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

Goblin::Goblin(std::string n, symbol s, TCODColor c, int h, int m, Weapon w, int a, int ws):
	Creature(n,s,c,h,m,w,a,ws)
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
	Goblin* copy = new Goblin(name, sym, color, maxHealth, maxMana, baseWeapon, baseAC, walkingSpeed);
	copy->health = health;
	copy->mana = mana;
	copy->controlled = controlled;
	copy->mainWeapon = mainWeapon;
	copy->attackSkill = attackSkill;
	copy->defenseSkill = defenseSkill;
	copy->level = level;
	copy->position = position;
	copy->walkingSpeed = walkingSpeed;
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
	store ("name", name) ("symbol", sym) ("position", position);
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
	store ("#inventory", (int) inventory.size());
	for (std::map<symbol, Item*>::iterator it = inventory.begin(); it != inventory.end(); it++)
	{
		store ("_symbol", (int) it->first) .ptr("_item", it->second->save(sg));
	}
	sg << store;
	return id;
}

// Note: Loading should work fine through Creature::load