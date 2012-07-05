#ifndef _OBJECT_H
#define _OBJECT_H

#include "utility.hpp"
#include <libtcod.hpp>

class Savegame;
class LoadBlock;
class Level;

enum OBJECTTYPE
{
  OBJ_STAIRSUP,
  OBJ_STAIRSDOWN,
  OBJ_STAIRSSAME,
  OBJ_DOOR_CLOSED,
  OBJ_DOOR_OPEN,
  OBJ_TRAP_BEAR,
  OBJ_TRAP_FIRE,
  NUM_OBJECTTYPES
};

class Object
{
private:
	OBJECTTYPE type;
	std::string name;
	uint formatFlags;
	symbol sym;
	TCODColor color;
	bool visible;

public:
	Object();
	Object(OBJECTTYPE type);
	Object(OBJECTTYPE type, symbol sym, TCODColor color, bool visible = true);
	uint getFormatFlags();
	OBJECTTYPE getType();
	symbol getSymbol();
	TCODColor getColor();
	bool isVisible();
	std::string toString();

	int onStep(Creature* guy);
	int onUse(Level* level, Point pos);
	bool isBlocking();
	bool isTransparent();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif