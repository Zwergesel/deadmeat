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
	RandomTable& add(std::string option, int relative_probability);
	std::string getRandom() const;
};

class InventoryTable
{
private:
	struct ChoiceList
	{
		int permill;
		int chooseMin;
		int chooseMax;
		std::vector<std::string> items;
		ChoiceList(int p, int lo, int hi):permill(p),chooseMin(lo),chooseMax(hi) {};
	};
	std::vector<ChoiceList> options;

public:
	InventoryTable();
	InventoryTable& add(const std::string& option, int permill);
	InventoryTable& add(const std::vector<std::string>& list, int permill, int min=0, int max=-1);
	std::vector<std::string> getRandom();
	//std::map<symbol,Item*> getRandomInventory();
};

struct CreatureTemplate
{
	Creature* creature;
};

class Factory
{

private:
	std::map<std::string, std::pair<Creature*, InventoryTable>> creatures;
	std::map<std::string, Item*> items;

public:
	Factory();
	Creature* spawnCreature(std::string monsterclass);
	Item* spawnItem(std::string itemclass);

	void setTemplate(std::string monsterclass, Creature* c, const InventoryTable& inv = InventoryTable());
	void setTemplate(std::string itemclass, Item* i);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);

};

extern Factory factory;

#endif