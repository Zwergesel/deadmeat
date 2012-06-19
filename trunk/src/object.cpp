#include "object.hpp"
#include "savegame.hpp"

Object::Object()
{
}

Object::Object(OBJECTTYPE t)
{
	type = t;
	switch (t)
	{
	case OBJ_STAIRSDOWN:
		sym = '>';
		color = TCODColor::brass;
		name = "stairs leading down";
		formatFlags=F_PLURAL;
		break;
	case OBJ_STAIRSUP:
		sym = '<';
		color = TCODColor::brass;
		name = "stairs leading up";
		formatFlags=F_PLURAL;
		break;
	default:
	case OBJ_STAIRSSAME:
		sym = '=';
		color = TCODColor::brass;
		name = "passage";
		formatFlags=F_DEFAULT;
		break;
	}
}

Object::Object(OBJECTTYPE t, symbol s, TCODColor c) : type(t), sym(s), color(c)
{
}

uint Object::getFormatFlags()
{
	return formatFlags;
}

OBJECTTYPE Object::getType()
{
	return type;
}

symbol Object::getSymbol()
{
	return sym;
}

TCODColor Object::getColor()
{
	return color;
}

std::string Object::toString()
{
	return name;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Object::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Object", id);
	store ("type", (int) type) ("name", name) ("formatFlags", formatFlags);
	store ("symbol", sym) ("color", color);
	sg << store;
	return id;
}

void Object::load(LoadBlock& load)
{
	int t;
	load ("type", t) ("name", name) ("formatFlags", formatFlags);
	load ("symbol", sym) ("color", color);
	if (t < 0 || t >= NUM_OBJECTTYPES) throw SavegameFormatException("Object::load _ type out of range");
	type = static_cast<OBJECTTYPE>(t);
}