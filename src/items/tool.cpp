#include "tool.hpp"
#include "../savegame.hpp"
#include "../world.hpp"
#include "../level.hpp"
#include "../player.hpp"

Tool::Tool()
{
	// constructor for savegames
	type = ITEM_TOOL;
	strType = "tools";
}

Tool::Tool(std::string n, uint f, symbol s, TCODColor c, int a, int w, ToolType t):
	Item(n,f,s,c,a,w), tool(t)
{
	type = ITEM_TOOL;
	strType = "tools";
}

Tool::~Tool() {}

Item* Tool::clone()
{
	Tool* copy = new Tool(name, formatFlags, sym, color, amount, weight, tool);
	copy->active = active;
	return copy;
}

bool Tool::requiresDirection()
{
	switch (tool)
	{
	default:
		return false;
	case TOOL_LOCKPICK:
		return true;
	}
}

int Tool::use(Level* level, Point position)
{
	if (tool == TOOL_LOCKPICK)
	{
		Object* obj = level->objectAt(position);
		if (obj != NULL && obj->getType() == OBJ_DOOR_LOCKED)
		{
			world.addMessage("You unlock the door.");
			obj->onUse(level, position);
			int dex = world.player->getAttribute(ATTR_DEX);
			return rng->getInt(50 - 2*dex, 200 - 4*dex);
		}
		else if (obj != NULL && obj->getType() == OBJ_DOOR_CLOSED)
		{
			world.addMessage("The door doesn't appear to be locked.");
		}
		else if (obj != NULL && obj->getType() == OBJ_DOOR_OPEN)
		{
			world.addMessage("This door is already open.");
		}
		else
		{
			world.addMessage("You see no door there.");
		}
	}
	return 0;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Tool::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Tool", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	store ("color", color) ("amount", amount) ("weight", weight) ("active", active);
	store ("tool", tool);
	sg << store;
	return id;
}

void Tool::load(LoadBlock& load)
{
	int t;
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	load ("color", color) ("amount", amount) ("weight", weight) ("active", active);
	load ("tool", t);
	if (t < 0 || tool >= NUM_TOOLTYPE) throw SavegameFormatException("Tool::load _ tool out of bounds");
	tool = static_cast<ToolType>(t);
}