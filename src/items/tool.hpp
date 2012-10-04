#ifndef _TOOL_HPP
#define _TOOL_HPP

#include "../item.hpp"
#include "utility.hpp"

class Savegame;
class LoadBlock;
class Level;

enum ToolType
{
  TOOL_LOCKPICK,
  TOOL_METAL,
  NUM_TOOLTYPE
};

class Tool : public Item
{
private:
	ToolType tool;

public:
	Tool();
	Tool(std::string name, uint format, symbol sym, TCODColor color, int amount, int weight, ToolType tool);
	~Tool();
	Item* clone();

	bool requiresDirection();
	int use(Level* level, Point position);

	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);
};

#endif