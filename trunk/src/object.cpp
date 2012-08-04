#include "object.hpp"
#include "savegame.hpp"
#include "creature.hpp"
#include "world.hpp"
#include "level.hpp"
#include "items/weapon.hpp"

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
		visible = true;
		break;
	case OBJ_STAIRSUP:
		sym = '<';
		color = TCODColor::brass;
		name = "stairs leading up";
		formatFlags = F_PLURAL;
		visible = true;
		break;
	case OBJ_STAIRSSAME:
		sym = '=';
		color = TCODColor::brass;
		name = "passage";
		formatFlags = F_DEFAULT;
		visible = true;
		break;
	case OBJ_DOOR_CLOSED:
		sym = 197;
		color = TCODColor::darkYellow;
		name = "door";
		formatFlags = F_DEFAULT;
		visible = true;
		break;
	case OBJ_DOOR_LOCKED:
		sym = 197;
		color = TCODColor::darkYellow;
		name = "door";
		formatFlags = F_DEFAULT;
		visible = true;
		break;
	case OBJ_DOOR_OPEN:
		sym = TCOD_CHAR_BLOCK1;
		color = TCODColor::darkYellow;
		name = "open door";
		formatFlags = F_AN;
		visible = true;
		break;
	case OBJ_DOOR_BROKEN:
		sym = TCOD_CHAR_BLOCK1;
		color = TCODColor::darkYellow;
		name = "broken door";
		formatFlags = F_DEFAULT;
		visible = true;
		break;
	case OBJ_TRAP_BEAR:
		sym = '^';
		color = TCODColor::cyan;
		name = "bear trap";
		formatFlags = F_DEFAULT;
		visible = false;
		break;
	case OBJ_TRAP_FIRE:
		sym = '^';
		color = TCODColor::red;
		name = "fire trap";
		formatFlags = F_DEFAULT;
		visible = false;
		break;
	default:
		sym = '#';
		color = TCODColor::pink;
		name = "OBJECT_ERROR";
		formatFlags = F_AN;
		visible = true;
		break;
	}
}

Object::Object(OBJECTTYPE t, symbol s, TCODColor c, bool v) : type(t), sym(s), color(c), visible(v)
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

bool Object::isVisible()
{
	return visible;
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
	case OBJ_DOOR_LOCKED:
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
	case OBJ_DOOR_LOCKED:
		return false;
	}
}

int Object::onStep(Creature* guy)
{
	std::stringstream msg;
	Point pos = guy->getPos();
	switch (type)
	{
	default:
		if (guy->isControlled())
		{
			// Inform player what he sees here
			msg << "There " << (formatFlags & F_PLURAL ? "are " : "is ");
			msg << util::format(FORMAT_INDEF, this) << " here.";
			world.addMessage(msg.str());
		}
		return 0;

	case OBJ_STAIRSSAME:
		if (guy->isControlled()) world.travel();
		return 0;

		// ==== TRAPS ==== //
	case OBJ_TRAP_BEAR:
		if (guy->isControlled())
		{
			world.addMessage("You get caught in a bear trap.", true);
			world.deathReason = "You were crushed by a bear trap.";
			visible = true;
		}
		else if (world.fovMap->isInFov(pos.x, pos.y))
		{
			msg << util::format(FORMAT_DEF, guy, true) << " gets caught in a bear trap.";
			world.addMessage(msg.str());
			visible = true;
		}
		guy->hurt(20, NULL, DAMAGE_WEAPON);
		guy->affect(STATUS_BEARTRAP, 0, 100, 1);
		return 0;

	case OBJ_TRAP_FIRE:
		if (guy->isControlled())
		{
			world.addMessage("Flames erupt from the ground and you catch on fire!", true);
			visible = true;
		}
		else if (world.fovMap->isInFov(pos.x, pos.y))
		{
			msg << util::format(FORMAT_DEF, guy, true) << " gets roasted in a fire trap.";
			world.addMessage(msg.str());
			visible = true;
		}
		guy->affect(STATUS_FIRE, 0, 100, 5);
		return 0;
	}
}

int Object::onUse(Level* level, Point pos)
{
	Creature* c;
	std::vector<Item*> items;
	std::stringstream msg;
	switch (type)
	{
	default:
		return 0;

	case OBJ_DOOR_OPEN:
		c = level->creatureAt(pos);
		items = level->itemsAt(pos);
		if (c == NULL && items.size() == 0)
		{
			*this = Object(OBJ_DOOR_CLOSED);
			world.addMessage("The door closes.");
		}
		else if (c != NULL)
		{
			msg << "You try to close the door, but " << util::format(FORMAT_DEF, c) << " is in the way.";
			world.addMessage(msg.str());
		}
		else
		{
			world.addMessage("You try to close the door, but there are items blocking it.");
		}
		return 15;

	case OBJ_DOOR_CLOSED:
		*this = Object(OBJ_DOOR_OPEN);
		world.addMessage("The door opens.");
		return 15;

	case OBJ_DOOR_LOCKED:
		*this = Object(OBJ_DOOR_CLOSED);
		return 0;
	}
}

bool Object::onAttack(Creature* guy, int attack, int damage, Weapon* weapon)
{
	TCODRandom rngGauss;
	rngGauss.setDistribution(TCOD_DISTRIBUTION_GAUSSIAN_RANGE);
	int hit;
	std::stringstream msg;

	if (type == OBJ_DOOR_CLOSED || type == OBJ_DOOR_LOCKED)
	{
		// Roll attack against the door; doors have 50 defense
		hit = util::clamp(attack + 950, 500, 1500);
		hit = rngGauss.getInt(700, 1300, hit);
		if (hit >= 1250 || (hit > 1000 && (1250 - hit) * damage >= 2000))
		{
			*this = Object(OBJ_DOOR_BROKEN);
			if (guy->isControlled()) world.addMessage("You hit the door really hard and destroy it.");
		}
		else
		{
			int broken = 3 - (hit / 740) - (hit / 790) - (hit / 850);
			bool didBreak = weapon->breakWeapon(broken);
			if (guy->isControlled())
			{
				msg << "You bash against the door with " << util::format(FORMAT_YOUR, weapon->getName(), weapon->getFormatFlags()) << ".";
				world.addMessage(msg.str());
				if (didBreak) world.addMessage("You hear a cracking sound.");
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Object::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Object", id);
	store ("type", (int) type) ("name", name) ("formatFlags", formatFlags);
	store ("symbol", sym) ("color", color) ("visible", visible);
	sg << store;
	return id;
}

void Object::load(LoadBlock& load)
{
	int t;
	load ("type", t) ("name", name) ("formatFlags", formatFlags);
	load ("symbol", sym) ("color", color) ("visible", visible);
	if (t < 0 || t >= NUM_OBJECTTYPES) throw SavegameFormatException("Object::load _ type out of range");
	type = static_cast<OBJECTTYPE>(t);
}