#include "../creature.hpp"
#include "../items/weapon.hpp"
#include "../utility.hpp"
#include "../world.hpp"
#include "../savegame.hpp"
#include "../level.hpp"
#include "../pathfinding.hpp"

BasicMonster::BasicMonster()
{
	// for savegames, initializes nothing
}

BasicMonster::BasicMonster(std::string n, uint f, symbol s, TCODColor c, int h, int m, Weapon w, int a, int ws, int exp, bool wm, bool wr):
	Creature(n,f,s,c,h,m,w,a,ws,exp),
	defaultUsesMeleeWeapons(wm), defaultUsesRangedWeapons(wr),
	lastSeenPlayer(Point(-1,-1))
{
}

BasicMonster::~BasicMonster()
{
	for (std::map<symbol, Item*>::iterator it=inventory.begin(); it!=inventory.end(); it++)
	{
		delete it->second;
	}
}

Creature* BasicMonster::clone()
{
	BasicMonster* copy = new BasicMonster();
	copy->copyFrom(this);
	return copy;
}

void BasicMonster::copyFrom(BasicMonster* original)
{
	Creature::copyFrom(original);
	defaultUsesMeleeWeapons = original->defaultUsesMeleeWeapons;
	defaultUsesRangedWeapons = original->defaultUsesRangedWeapons;
	lastSeenPlayer = original->lastSeenPlayer;
}

bool BasicMonster::usesMeleeWeapons()
{
	return defaultUsesMeleeWeapons;
}

bool BasicMonster::usesRangedWeapons()
{
	return defaultUsesRangedWeapons;
}

void BasicMonster::useBestWeapon()
{
	bool melee = usesMeleeWeapons();
	bool ranged = usesRangedWeapons();
	int bestScore = -1;
	symbol bestWeapon = '\0';
	
	for (auto it = inventory.begin(); it != inventory.end(); it++)
	{
		if (it->second->getType() != ITEM_WEAPON) continue;
		Weapon* weapon = static_cast<Weapon*>(it->second);
		if (weapon->getRange() <= 1 && !melee) continue;
		if (weapon->getRange() > 1 && !ranged) continue;
		int score = scoreWeapon(weapon);
		if (score > bestScore)
		{
			bestScore = score;
			bestWeapon = it->first;
		}
	}
	
	mainWeapon = bestWeapon;
}

int BasicMonster::scoreWeapon(Weapon* weapon)
{
	return weapon->getDPS();
}

bool BasicMonster::shouldFlee()
{
	return ((maxHealth / health) >= 5);
}

bool BasicMonster::seePlayer()
{
	// TODO: sight radius
	if (world.fovMap->isInFov(position.x, position.y)) return true;
	
	// Trace line
	Point target = world.player->getCreature()->getPos();
	Point current = position;
	TCODLine::init(current.x, current.y, target.x, target.y);
	do
	{
		if (!level->isTransparent(current)) return false;
	}
	while (!TCODLine::step(&current.x, &current.y));
	return true;
}

int BasicMonster::doRangedAttack(Weapon* weapon)
{
	return -1;
}

int BasicMonster::doSpecialAttack()
{
	return -1;
}

int BasicMonster::doMeleeAttack(Weapon* weapon)
{
	Point target = world.player->getCreature()->getPos();
	if (Point::sqlen(target - position) > 2) return -1;
	return attack(target);
}

int BasicMonster::doChargePlayer()
{
	lastSeenPlayer = world.player->getCreature()->getPos();
	return navigateTo(lastSeenPlayer);
}

int BasicMonster::doWander()
{
	if (position == lastSeenPlayer) lastSeenPlayer = Point(-1,-1);
	if (lastSeenPlayer.x >= 0)
	{
		return navigateTo(lastSeenPlayer);
	}
	else
	{
		// TODO: random walk here
		return 10;
	}
}

int BasicMonster::navigateTo(Point target)
{
	// TODO: One centralized dijkstra should improve performance significantly
	TCODPath path = TCODPath(level->getWidth(), level->getHeight(), new PathFindingCallback(), level);
	path.compute(position.x, position.y, target.x, target.y);

	Point step;
	if (!path.isEmpty() && path.walk(&step.x, &step.y, true))
	{
		Creature* c = level->creatureAt(step);
		if (c != NULL)
		{
			// TODO: more elegant solution?
			return 10;
		}
		else if (getStatusStrength(STATUS_BEARTRAP) > 0)
		{
			// TODO: resolve bear trap and other statuses somewhere else
			if (rng->getInt(0,9) == 0) endStatus(STATUS_BEARTRAP);
			return 10;
		}
		else
		{
			float diagonal = (step.x - position.x != 0 && step.y - position.y != 0) ? std::sqrt(2.f) : 1.f;
			position = step;
			Object* obj = level->objectAt(step);
			if (obj != NULL) obj->onStep(this);
			return static_cast<int>(getWalkingSpeed() * diagonal);
		}
	} else {
		return 0;
	}
}

int BasicMonster::action()
{
	if ((usesMeleeWeapons() || usesRangedWeapons()) && inventory.size() > 0)
	{
		useBestWeapon();
	}
	if (shouldFlee())
	{
		// TODO: Flee and return correct time
		return 10;
	}
	int time;
	if (seePlayer())
	{
		int sqdist = Point::sqlen(world.player->getCreature()->getPos() - position);
		Weapon* weapon = getMainWeapon();
		if (weapon == NULL) weapon = &baseWeapon;
		int range = weapon->getRange();
		
		// Ranged attack
		if (range > 1 && sqdist <= range*range)
		{
			time = doRangedAttack(weapon);
			if (time > 0) return time;
		}
		
		// Special attacks (e.g. dragon breath)
		time = doSpecialAttack();
		if (time > 0) return time;
		
		// Melee attack
		if (sqdist <= 2)
		{
			time = doMeleeAttack(weapon);
			if (time > 0) return time;
		}
		
		// Run towards player / tactical movement
		time = doChargePlayer();
		if (time > 0) return time;
	}
	
	// Random walk, search for items, etc.
	time = doWander();
	return time > 0 ? time : 10;
}

/*--------------------- SAVING AND LOADING ---------------------*/

void BasicMonster::storeAll(Savegame& sg, SaveBlock& store)
{
	Creature::storeAll(sg, store);
	store ("lastSeenPlayer", lastSeenPlayer) ("defaultUsesMeleeWeapons", defaultUsesMeleeWeapons);
	store ("defaultUsesRangedWeapons", defaultUsesRangedWeapons);
}

unsigned int BasicMonster::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this, &id)) return id;
	SaveBlock store("BasicMonster", id);
	storeAll(sg, store);
	sg << store;
	return id;
}

void BasicMonster::load(LoadBlock& load)
{
	Creature::load(load);
	load ("lastSeenPlayer", lastSeenPlayer) ("defaultUsesMeleeWeapons", defaultUsesMeleeWeapons);
	load ("defaultUsesRangedWeapons", defaultUsesRangedWeapons);
}