#include <libtcod.hpp>
#include <sstream>
#include "player.hpp"
#include "creature.hpp"
#include "level.hpp"
#include "world.hpp"

Skill::Skill()
{
}

Skill::Skill(std::string name, int value, ATTRIBUTE att)
{
	this->name = name;
	this->value = value;
	this->att = att;
	this->exp = 0;
	this->used = 0;
}

Player::Player(std::string name):
	name(name)
{
	skills[SKILL_MELEE_COMBAT] = Skill("melee combat", 0, ATTR_STR);
	skills[SKILL_RANGED_COMBAT] = Skill("ranged combat", 0, ATTR_DEX);
	skills[SKILL_HEALTH] = Skill("health", 0, ATTR_CON);
	skills[SKILL_UNARMORED] = Skill("unarmored", 0, ATTR_DEX);
	skills[SKILL_LEATHER_ARMOR] = Skill("leather armor", 0, ATTR_DEX);
	skills[SKILL_SCALE_ARMOR] = Skill("scale armor", 0, ATTR_DEX);
	skills[SKILL_RING_ARMOR] = Skill("ring armor", 0, ATTR_DEX);
	skills[SKILL_CLOTH_ARMOR] = Skill("cloth armor", 0, ATTR_DEX);
	skills[SKILL_PLATE_ARMOR] = Skill("plate armor", 0, ATTR_DEX);
	skills[SKILL_UNARMED] = Skill("unarmed", 0, ATTR_STR);
	skills[SKILL_AXE] = Skill("axe", 0, ATTR_STR);
	skills[SKILL_SWORD] = Skill("sword", 0, ATTR_STR);
	skills[SKILL_MACEFLAIL] = Skill("mace & flail", 0, ATTR_STR);
	skills[SKILL_STAFF] = Skill("staff", 0, ATTR_STR);
	skills[SKILL_DAGGER] = Skill("dagger", 0, ATTR_DEX);
	skills[SKILL_WHIP] = Skill("whip", 0, ATTR_DEX);
	skills[SKILL_PIKE] = Skill("pike", 0, ATTR_STR);
	skills[SKILL_BOW] = Skill("bow", 0, ATTR_DEX);
	skills[SKILL_CROSSBOW] = Skill("crossbow", 0, ATTR_DEX);
	skills[SKILL_SLING] = Skill("sling", 0, ATTR_DEX);
	creature = new Creature(Point(40,22), name, '@', TCODColor::black, 25);
	creature->setControlled(true);
}

int Player::attack(int& attack, int& damage, int& speed)
{
	// weapon damage + enchantment
	damage = std::max(1, 10 + 0);
	// base speed - weapon speed + armor hindrance
	speed = std::max(1, 10 - 2 + 2);
	// weapon attack + weapon enchantment + (melee combat skill + weapon skill)/2
	attack = 10 + 0 + (skills[SKILL_MELEE_COMBAT].value + skills[/*weapon.getSkill()*/SKILL_UNARMED].value)/2;

	return speed;
}

int Player::getDefense()
{
	// armor + (fighting skill + armor skill)/2 + tile defender is on
	return 0 + (skills[SKILL_MELEE_COMBAT].value + skills[/*armor.getSkill()*/SKILL_UNARMORED].value)/2 + 0;
}

Creature* Player::getCreature()
{
	return creature;
}

TCOD_key_t Player::waitForKeypress(bool clBuf)
{
	while (true)
	{
		TCOD_key_t key = TCODConsole::root->waitForKeypress(clBuf);
		if (TCODConsole::isWindowClosed() || (key.pressed && (key.lalt || key.ralt) && key.vk == TCODK_F4))
		{
			world.requestQuit = true;
			return key;
		}
		else if (key.pressed && (key.lalt || key.ralt) && key.vk == TCODK_ENTER)
		{
			world.toggleFullscreen();
		}
		else if (key.pressed)
		{
			return key;
		}
	}
	return TCOD_key_t();
}

int Player::action(Level* level)
{
	do
	{
		TCOD_key_t key = waitForKeypress(true);
		bool move(false);
		int direction(0);

		if (key.vk >= TCODK_KP1 && key.vk <= TCODK_KP9 && key.vk != TCODK_KP5)
		{
			// numpad player movement
			move = true;
			direction = key.vk - TCODK_KP1;
		}
		else if (key.vk >= TCODK_1 && key.vk <= TCODK_9 && key.vk != TCODK_5)
		{
			// number keys player movement
			move = true;
			direction = key.vk - TCODK_1;
		}
		else if (key.vk == TCODK_5 || key.vk == TCODK_KP5)
		{
			// wait/search
			return 10;
		}
		else if (key.c == ':')
		{
			std::stringstream msg;
			std::vector<Item*> items = level->itemsAt(creature->getPos());
			if (items.size() == 1)
			{
				msg << "You see a " << items[0]->getName() << " here.";
				world.addMessage(msg.str());
			}
			else if (items.size() >= 1)
			{
				msg << "You see a several items here:";
				world.addMessage(msg.str());
				for (std::vector<Item*>::iterator it=items.begin(); it<items.end(); it++)
				{
					std::stringstream strlist;
					strlist << util::indefArticle((*it)->getName()) << " " << (*it)->getName();
					world.addMessage(strlist.str());
				}
			}
			return 10;
		}

		if (move)
		{
			// execute movement
			Point ppos = creature->getPos();
			Point newPos = Point(ppos.x + Player::dx[direction], ppos.y + Player::dy[direction]);
			if (newPos.x >= 0 && newPos.y < level->getWidth() && newPos.y >= 0 && newPos.y < level->getHeight())
			{
				Creature* c = level->creatureAt(newPos);
				if (c != NULL)
				{
					return creature->attack(c);
				}
				else if (world.tileSet->isPassable(level->getTile(newPos)))
				{
					creature->moveTo(newPos);
					world.levelOffset.x = util::clamp(world.viewLevel.width/2 - newPos.x, world.viewLevel.width - level->getWidth(), 0);
					world.levelOffset.y = util::clamp(world.viewLevel.height/2 - newPos.y, world.viewLevel.height - level->getHeight(), 0);
					return 12;
				}
				else
				{
					world.addMessage("Bonk!");
					return 0;
				}
			}
		}
	}
	while (!TCODConsole::isWindowClosed() && !world.requestQuit);
	// should not be reached, return time of action
	return 1;
}

int Player::dx[] = {-1,0,1,-1,0,1,-1,0,1};
int Player::dy[] = {1,1,1,0,0,0,-1,-1,-1};