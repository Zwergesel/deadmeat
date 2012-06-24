#include "object.hpp"
#include "savegame.hpp"
#include "creature.hpp"
#include "world.hpp"

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
		formatFlags = F_PLURAL;
		break;
	case OBJ_STAIRSUP:
		sym = '<';
		color = TCODColor::brass;
		name = "stairs leading up";
		formatFlags = F_PLURAL;
		break;
	case OBJ_STAIRSSAME:
		sym = '=';
		color = TCODColor::brass;
		name = "passage";
		formatFlags = F_DEFAULT;
		break;
	case OBJ_DOOR_CLOSED:
		sym = 197;
		color = TCODColor::darkYellow;
		name = "door";
		formatFlags = F_DEFAULT;
		break;
	default:
	case OBJ_DOOR_OPEN:
		sym = TCOD_CHAR_BLOCK1;
		color = TCODColor::darkYellow;
		name = "open door";
		formatFlags = F_AN;
		break;
	case OBJ_TRAP_BEAR:
		sym = '^';
		color = TCODColor::cyan;
		name = "bear trap";
		formatFlags = F_DEFAULT;
		break;
	case OBJ_TRAP_FIRE:
		sym = '^';
		color = TCODColor::red;
		name = "fire trap";
		formatFlags = F_DEFAULT;
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

bool Object::isBlocking()
{
	switch (type)
	{
	default:
		return false;
	case OBJ_DOOR_CLOSED:
		return true;
	}
}

bool Object::isTransparent()
{
	switch (type)
	{
	default:
		return true;
	case OBJ_DOOR_CLOSED:
		return false;
	}
}

int Object::onStep(Creature* guy)
{
	std::stringstream msg;
	switch (type)
	{
	default:
		return 0;
	case OBJ_STAIRSSAME:
		if (guy->isControlled()) world.travel();
		return 0;

		// TRAPS
	case OBJ_TRAP_BEAR:
		if (guy->isControlled()) msg << "You get caught in a bear trap.";
		else msg << util::format(FORMAT_DEF, guy, true) << " gets caught in a bear trap.";
		world.addMessage(msg.str());
		guy->hurt(50, NULL);
		return 0;
	case OBJ_TRAP_FIRE:
		if (guy->isControlled()) msg << "You get roasted by a fire trap.";
		else msg << util::format(FORMAT_DEF, guy, true) << " gets roasted by a fire trap.";
		world.addMessage(msg.str());
		guy->hurt(50, NULL);
		return 0;
	}
}

int Object::onUse()
{
	std::stringstream msg;
	switch (type)
	{
	default:
		return 0;
	case OBJ_DOOR_OPEN:
		*this = Object(OBJ_DOOR_CLOSED);
		world.addMessage("The door closes.");
		return 15;
	case OBJ_DOOR_CLOSED:
		*this = Object(OBJ_DOOR_OPEN);
		world.addMessage("The door opens.");
		return 15;
	}
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