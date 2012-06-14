#ifndef _OBJECT_H
#define _OBJECT_H

#include "utility.hpp"
#include <libtcod.hpp>

enum OBJECTTYPE
{
  OBJECTTYPE_STAIRSUP,
  OBJECTTYPE_STAIRSDOWN,
  OBJECTTYPE_STAIRSSAME,
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

public:
	Object();
	Object(OBJECTTYPE type);
	Object(OBJECTTYPE type, symbol sym, TCODColor color);
	uint getFormatFlags();
	OBJECTTYPE getType();
	symbol getSymbol();
	TCODColor getColor();
	std::string toString();
};

#endif