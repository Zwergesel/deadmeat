#include <cassert>
#include "factory.hpp"
#include "creature.hpp"
#include "item.hpp"
#include "savegame.hpp"

RandomTable::RandomTable():
	probTotal(0)
{
}

void RandomTable::add(std::string option, int prob)
{
	if (prob <= 0) return;
	probTotal += prob;
	options.push_back(std::pair<std::string,int>(option,prob));
}

std::string RandomTable::getRandom() const
{
	TCODRandom* rng = TCODRandom::getInstance();
	rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);
	int roll = rng->getInt(1,probTotal);
	for (auto it=options.begin(); it!=options.end(); it++)
	{
		roll -= it->second;
		if (roll <= 0) return it->first;
	}
	return "";
}

void InventoryTable::add(const std::string& option, int permill)
{
	assert(0 < permill && permill <= 1000);
	ChoiceList in(permill, 1, 1);
	in.items.push_back(option);
	options.push_back(in);
}

void InventoryTable::add(const std::vector<std::string>& list, int permill, int min, int max)
{
	if (max < 0) max = list.size();
	assert(min >= 0);
	assert(min < max);
	assert((uint) max <= list.size());
	assert(0 < permill && permill <= 1000);
	ChoiceList in(permill, min, max);
	in.items.insert(in.items.begin(), list.begin(), list.end());
	options.push_back(in);
}

std::vector<std::string> InventoryTable::getRandom()
{
	std::vector<std::string> result;
	TCODRandom* rng = TCODRandom::getInstance();
	rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);
	for (auto it = options.begin(); it != options.end(); it++)
	{
		int count = it->chooseMin == it->chooseMax ? it->chooseMin : rng->getInt(it->chooseMin, it->chooseMax);
		for (int i=1; i<=count; i++)
		{
			swap(it->items.at(rng->getInt(0, it->items.size()-count)), it->items.back());
			result.push_back(it->items.back());
		}
	}
	return result;
}

Factory::Factory()
{
}

Creature* Factory::spawnCreature(std::string monsterclass)
{
	auto it = creatures.find(monsterclass);
	if (it == creatures.end()) return NULL;
	return it->second->clone();
}

void Factory::setTemplate(std::string monsterclass, Creature* c)
{
	Creature* nc = c->clone();
	auto it = creatures.find(monsterclass);
	if (it != creatures.end())
	{
		delete it->second;
		it->second = nc;
	}
	else
	{
		creatures.insert(it, std::make_pair(monsterclass, nc));
	}
}

Item* Factory::spawnItem(std::string itemclass)
{
	auto it = items.find(itemclass);
	if (it == items.end()) return NULL;
	return it->second->clone();
}

void Factory::setTemplate(std::string itemclass, Item* i)
{
	Item* ni = i->clone();
	auto it = items.find(itemclass);
	if (it != items.end())
	{
		delete it->second;
		it->second = ni;
	}
	else
	{
		items.insert(it, std::make_pair(itemclass, ni));
	}
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Factory::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Factory", id);
	store ("#creatures", (int) creatures.size());
	for (auto it = creatures.begin(); it != creatures.end(); it++)
	{
		store("_spawnName", it->first).ptr("_creature", it->second->save(sg));
	}
	store ("#items", (int) items.size());
	for (auto it = items.begin(); it != items.end(); it++)
	{
		store("_spawnName", it->first).ptr("_item", it->second->save(sg));
	}
	sg << store;
	return id;
}

void Factory::load(LoadBlock& load)
{
	// Delete previous templates
	for (auto it = creatures.begin(); it != creatures.end(); it++)
	{
		if (it->second != NULL)
		{
			delete it->second;
		}
	}
	for (auto it = items.begin(); it != items.end(); it++)
	{
		if (it->second != NULL)
		{
			delete it->second;
		}
	}
	creatures.clear();
	items.clear();
	
	// Load new templates
	int n;
	load ("#creatures", n);
	while (n-->0)
	{
		std::string name;
		load("_spawnName", name);
		creatures[name] = static_cast<Creature*>(load.ptr("_creature"));
	}
	load ("#items", n);
	while (n-->0)
	{
		std::string name;
		load("_spawnName", name);
		items[name] = static_cast<Item*>(load.ptr("_item"));
	}
}