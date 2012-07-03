#include <cassert>
#include "factory.hpp"
#include "creature.hpp"
#include "item.hpp"
#include "savegame.hpp"

RandomTable::RandomTable():
	probTotal(0)
{
}

RandomTable& RandomTable::add(std::string option, int prob)
{
	if (prob <= 0) return *this;
	probTotal += prob;
	options.push_back(std::pair<std::string,int>(option,prob));
	return *this;
}

std::string RandomTable::getRandom() const
{
	int roll = rng->getInt(1,probTotal);
	for (auto it=options.begin(); it!=options.end(); it++)
	{
		roll -= it->second;
		if (roll <= 0) return it->first;
	}
	return "";
}

InventoryTable::InventoryTable()
{
}

InventoryTable& InventoryTable::add(const std::string& option, int permill)
{
	assert(0 < permill && permill <= 1000);
	ChoiceList in(permill, 1, 1);
	in.items.push_back(option);
	options.push_back(in);
	return *this;
}

InventoryTable& InventoryTable::add(const std::vector<std::string>& list, int permill, int min, int max)
{
	if (max < 0) max = list.size();
	assert(min >= 0);
	assert(min <= max);
	assert((uint) max <= list.size());
	assert(0 < permill && permill <= 1000);
	ChoiceList in(permill, min, max);
	in.items.assign(list.begin(), list.end());
	options.push_back(in);
	return *this;
}

std::vector<std::string> InventoryTable::getRandom()
{
	std::vector<std::string> result;
	for (auto it = options.begin(); it != options.end(); it++)
	{
		if (it->permill == 1000 || rng->getInt(0,999) < it->permill)
		{
			int count = it->chooseMin == it->chooseMax ? it->chooseMin : rng->getInt(it->chooseMin, it->chooseMax);
			for (int i=1; i<=count; i++)
			{
				swap(it->items.at(rng->getInt(0, it->items.size()-count)), it->items.back());
				result.push_back(it->items.back());
			}
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
	assert(it != creatures.end());
	Creature* spawned = it->second.first->clone();
	std::vector<std::string> items = it->second.second.getRandom();
	for (auto it=items.begin(); it!=items.end(); it++)
	{
		spawned->addItem(spawnItem(*it, true));
	}
	return spawned;
}

void Factory::setTemplate(std::string monsterclass, Creature* c, const InventoryTable& inv)
{
	Creature* nc = c->clone();
	auto it = creatures.find(monsterclass);
	if (it != creatures.end())
	{
		delete it->second.first;
		it->second.first = nc;
		it->second.second = inv;
	}
	else
	{
		creatures.insert(it, std::make_pair(monsterclass, std::make_pair(nc, inv)));
	}
}

Item* Factory::spawnItem(std::string itemclass, bool randomize)
{
	auto it = items.find(itemclass);
	assert(it != items.end());
	Item* item = it->second->clone();
	if (randomize) item->randomize(0);
	return item;
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
		store("_spawnName", it->first).ptr("_creature", it->second.first->save(sg));
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
		if (it->second.first != NULL)
		{
			delete it->second.first;
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
		creatures[name] = std::make_pair(static_cast<Creature*>(load.ptr("_creature")), InventoryTable());
	}
	load ("#items", n);
	while (n-->0)
	{
		std::string name;
		load("_spawnName", name);
		items[name] = static_cast<Item*>(load.ptr("_item"));
	}
}