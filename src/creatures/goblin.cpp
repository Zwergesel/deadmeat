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
	copy->lastTimeManaRegen = lastTimeManaRegen;
	copy->lastPlayerPos = lastPlayerPos;
	copy->seenPlayer = seenPlayer;
	copy->expValue = expValue;
	copy->quiver = quiver;
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
	regenerate(0, 0);

	// pick best weapon
	symbol choice = '0';
	float value = baseWeapon.getDPS();
	for (auto it=inventory.begin(); it!=inventory.end(); it++)
	{
		if (it->second->getType() != ITEM_WEAPON) continue;
		Weapon* w = static_cast<Weapon*>(it->second);
		if (w->getRange() > 1) continue; // TODO: use ranged weapons against the player too
		if (w->getDPS() > value)
		{
			choice = it->first;
			value = w->getDPS();
		}
	}
	mainWeapon = choice;

	Point ppos = world.player->getCreature()->getPos();
	// can the goblin see the player?
	bool seesPlayer = true;
	Point p = position;
	TCODLine::init(p.x, p.y, ppos.x, ppos.y);
	do
	{
		if (!level->isTransparent(p))
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
		else if (getStatusStrength(STATUS_BEARTRAP) > 0)
		{
			if (rng->getInt(0,9) == 0) endStatus(STATUS_BEARTRAP);
		}
		else
		{
			float diagonal = ((target - position).x != 0 && (target - position).y != 0)?(std::sqrt(2.f)):(1.f);
			position = target;
			Object* obj = level->objectAt(target);
			if (obj != NULL) obj->onStep(this);
			return static_cast<int>(static_cast<float>(getWalkingSpeed()) * diagonal);
		}
	}
	return walkingSpeed + getHindrance();
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Goblin::save(Savegame& sg)
{
	return Creature::save(sg, "Goblin");
}

// Note: Loading should work fine through Creature::load