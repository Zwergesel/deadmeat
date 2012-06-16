#include "object.hpp"

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