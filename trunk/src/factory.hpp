#ifndef _MONSTERFACTORY_HPP
#define _MONSTERFACTORY_HPP

/* Includes */

#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include "utility.hpp"

/* Forward declarations */

class Creature;
class Item;
class Savegame;
class LoadBlock;

/* Actual classes */

struct RandomTable
{
	int probTotal;
	std::vector<std::pair<std::string,int>> options;
	RandomTable();
	void add(std::string option, int relative_probability);
	std::string getRandom() const;
};

class InventoryTable
{
private:
	struct ChoiceList
	{
		int permill;
		int choose;
		std::vector<std::string> items;
	};
	std::vector<ChoiceList> options;
	
public:
	InventoryTable();
	void add(std::string option, int permill);
	void add(const std::vector<std::string>& list, int choose, int permill);
	std::vector<std::string> getRandom();
	std::map<symbol,Item*> getRandomInventory();
};

struct CreatureTemplate
{
	Creature* creature;
};

class Factory
{

private:
	std::map<std::string, Creature*> creatures;
	std::map<std::string, Item*> items;

public:
	Factory();
	Creature* spawnCreature(std::string monsterclass);
	Item* spawnItem(std::string itemclass);
	
	void setTemplate(std::string monsterclass, Creature* c);
	void setTemplate(std::string itemclass, Item* i);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);

};

extern Factory factory;

#endif