#ifndef _MONSTERFACTORY_HPP
#define _MONSTERFACTORY_HPP

/* Includes */

#include <map>
#include <vector>
#include <string>
#include <algorithm>

/* Forward declarations */

class Creature;
class Savegame;
class LoadBlock;

/* Actual classes */

struct SpawnList
{
	int probTotal;
	std::vector<std::pair<std::string,int>> monsters;
	SpawnList();
	void add(std::string monsterclass, int relative_probability);
	std::string getRandom() const;
};

struct CreatureTemplate
{
	Creature* creature;
};

class Factory
{

private:
	std::map<std::string, Creature*> templates;

public:
	Factory();
	Creature* spawnCreature(std::string monsterclass);
	void setTemplate(std::string monsterclass, Creature* c);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);

};

extern Factory factory;

#endif