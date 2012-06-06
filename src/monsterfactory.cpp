#include "monsterfactory.hpp"
#include "creature.hpp"
#include "savegame.hpp"

MonsterFactory::MonsterFactory()
{
}

Creature* MonsterFactory::spawnCreature(std::string monsterclass)
{
	std::map<std::string, Creature*>::iterator it = templates.find(monsterclass);
	if (it == templates.end()) return NULL;
	return it->second->clone();
}

void MonsterFactory::setTemplate(std::string monsterclass, Creature* c)
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

unsigned int MonsterFactory::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("MonsterFactory", id);
	store ("#templates", (int) templates.size());
	for (std::map<std::string, Creature*>::iterator it = templates.begin(); it != templates.end(); it++)
	{
		store("_spawnName", it->first).ptr("_creature", it->second->save(sg));
	}
	sg << store;
	return id;
}

void MonsterFactory::load(LoadBlock& load)
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