#include <cassert>
#include "factory.hpp"
#include "creature.hpp"
#include "savegame.hpp"

SpawnList::SpawnList():
	probTotal(0)
{
}

void SpawnList::add(std::string monsterclass, int prob)
{
	if (prob <= 0) return;
	probTotal += prob;
	monsters.push_back(std::pair<std::string,int>(monsterclass,prob));
}

std::string SpawnList::getRandom() const
{
	TCODRandom* rng = TCODRandom::getInstance();
	rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);
	int roll = rng->getInt(1,probTotal);
	for (auto it=monsters.begin(); it!=monsters.end(); it++)
	{
		roll -= it->second;
		if (roll <= 0) return it->first;
	}
	assert(false);
	return "";
}

Factory::Factory()
{
}

Creature* Factory::spawnCreature(std::string monsterclass)
{
	std::map<std::string, Creature*>::iterator it = templates.find(monsterclass);
	if (it == templates.end()) return NULL;
	return it->second->clone();
}

void Factory::setTemplate(std::string monsterclass, Creature* c)
{
	Creature* nc = c->clone();
	std::map<std::string, Creature*>::iterator it = templates.find(monsterclass);
	if (it != templates.end())
	{
		delete it->second;
		it->second = nc;
	}
	else
	{
		templates.insert(it, std::make_pair(monsterclass, nc));
	}
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Factory::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Factory", id);
	store ("#templates", (int) templates.size());
	for (std::map<std::string, Creature*>::iterator it = templates.begin(); it != templates.end(); it++)
	{
		store("_spawnName", it->first).ptr("_creature", it->second->save(sg));
	}
	sg << store;
	return id;
}

void Factory::load(LoadBlock& load)
{
	// Delete previous templates
	for (std::map<std::string, Creature*>::iterator it = templates.begin(); it != templates.end(); it++)
	{
		if (it->second != NULL)
		{
			delete it->second;
		}
	}
	templates.clear();
	// Load new templates
	int n;
	load ("#templates", n);
	while (n-->0)
	{
		std::string name;
		load("_spawnName", name);
		templates[name] = static_cast<Creature*>(load.ptr("_creature"));
	}
}