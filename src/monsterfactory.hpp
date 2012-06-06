#ifndef _MONSTERFACTORY_HPP
#define _MONSTERFACTORY_HPP

/* Includes */

#include <map>
#include <string>

/* Forward declarations */

class Creature;
class Savegame;
class LoadBlock;

/* Actual class */

class MonsterFactory
{

private:
	std::map<std::string, Creature*> templates;

public:
	MonsterFactory();
	Creature* spawnCreature(std::string monsterclass);
	void setTemplate(std::string monsterclass, Creature* c);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);

};

extern MonsterFactory monsterfactory;

#endif