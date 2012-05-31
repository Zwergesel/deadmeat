#include <libtcod.hpp>
#include <sstream>
#include <cassert>
#include "player.hpp"
#include "creature.hpp"
#include "level.hpp"
#include "world.hpp"
#include "item.hpp"
#include "items/weapon.hpp"

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

int Player::dx[] = {-1,0,1,-1,0,1,-1,0,1};
int Player::dy[] = {1,1,1,0,0,0,-1,-1,-1};

Player::Player(std::string name):
	name(name),
	state(STATE_DEFAULT)
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
	creature = new Creature(Point(40,22), name, '@', TCODColor::black, 250);
	creature->setControlled(true);
	creature->setAttackSkill(skills[SKILL_UNARMED].value);
	creature->setBaseWeapon(Weapon(Point(0,0), "hands", '§', TCODColor::pink, 10, 10, 10, 0, 0, 0, SKILL_UNARMED, 2));
}

Player::~Player()
{
	for (std::vector<std::pair<int, Item*> >::iterator it=inventory.begin(); it<inventory.end(); it++)
	{
		if ((*it).second != NULL)
		{
			delete (*it).second;
			(*it).second = NULL;
		}
	}
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

bool Player::addItem(Item* item)
{
	bool used[52];
	for (int i=0; i<52; i++) used[i] = false;
	for (std::vector<std::pair<int, Item*> >::iterator it=inventory.begin(); it<inventory.end(); it++)
	{
		used[(*it).first] = true;
	}
	for (int i=0; i<52; i++)
	{
		if (!used[i])
		{
			inventory.push_back(std::pair<int, Item*>(i, item));
			return true;
		}
	}
	// ran out of letters
	world.addMessage("Too many items in backpack.");
	return false;
}

void Player::removeItem(Item* item, bool del)
{
	for (std::vector<std::pair<int, Item*> >::iterator it=inventory.begin(); it<inventory.end(); it++)
	{
		if ((*it).second == item)
		{
			if ((*it).second != NULL && del)
			{
				delete (*it).second;
				(*it).second = NULL;
			}
			inventory.erase(it);
			return;
		}
	}
}

std::vector<std::pair<int, Item*> > Player::getInventory()
{
	return inventory;
}

Item* Player::getInventoryItem(int item)
{
	for (InventoryIterator it = inventory.begin(); it != inventory.end(); it++)
	{
		if (it->first == item)
		{
			return it->second;
		}
	}
	return NULL;
}

int Player::actionMove(int direction)
{
	Level* level = world.levels[world.currentLevel];
	Point ppos = creature->getPos();
	Point newPos = Point(ppos.x + Player::dx[direction], ppos.y + Player::dy[direction]);
	if (newPos.x >= 0 && newPos.x < level->getWidth() && newPos.y >= 0 && newPos.y < level->getHeight())
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
	return 0;
}

int Player::actionLook(Point pos)
{
	Level* level = world.levels[world.currentLevel];
	std::stringstream msg;
	std::vector<Item*> items = level->itemsAt(pos);
	if (items.size() == 1)
	{
		msg << "You see " << util::indefArticle(items[0]->getName()) << " " << items[0]->getName() << " here.";
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
	return 0;
}

int Player::actionPickup()
{
	Level* level = world.levels[world.currentLevel];
	std::stringstream msg;
	std::vector<Item*> items = level->itemsAt(creature->getPos());
	if (items.size() <= 0)
	{
		world.addMessage("Nothing here.");
		return 0;
	}
	else if (items.size() == 1)
	{
		return actionPickup(items[0]);
	}
	else
	{
		state = STATE_PICKUP;
		world.itemSelection = ItemSelection(items, "Pick up what?", true);
		world.itemSelection.compile(world.viewLevel.height - world.viewLevel.height / 4 - 6);
		return 0;
	}
	return 0;
}

int Player::actionPickup(Item* item)
{
	Level* level = world.levels[world.currentLevel];
	addItem(item);
	{
		std::stringstream msg;
		msg << "Picked up " << util::indefArticle(item->getName()) << " " << item->getName() << ".";
		world.addMessage(msg.str());
		level->removeItem(item, false);
		return 10;
	}
	return 0;
}

int Player::actionWield(Item* itemObj)
{
	//Item* itemObj = getInventoryItem(item);
	std::stringstream msg;
	if (itemObj == NULL) return 0;
	assert(itemObj->getType() == ITEM_WEAPON);

	Weapon* weapon = static_cast<Weapon*>(itemObj);
	if (creature->getMainWeapon() == weapon)
	{
		msg << "You were already wielding a " << weapon->toString() << ".";
		world.addMessage(msg.str());
		return 0;
	}
	else
	{
		creature->wieldMainWeapon(weapon, computeAttackBonus(weapon));
		msg << "You are now wiedling a " << weapon->toString() << ".";
		world.addMessage(msg.str());
		return 30;
	}
}

int Player::action()
{
	do
	{
		TCOD_key_t key = waitForKeypress(true);

		// numpad player movement
		if (state == STATE_DEFAULT && key.vk >= TCODK_KP1 && key.vk <= TCODK_KP9 && key.vk != TCODK_KP5)
		{
			return actionMove(key.vk - TCODK_KP1);
		}
		// number keys player movement
		else if (state == STATE_DEFAULT && key.vk >= TCODK_1 && key.vk <= TCODK_9 && key.vk != TCODK_5)
		{
			return actionMove(key.vk - TCODK_1);
		}
		else if (state == STATE_DEFAULT && (key.vk == TCODK_5 || key.vk == TCODK_KP5))
		{
			// wait/search
			return 10;
		}
		// look at current position
		else if (state == STATE_DEFAULT && key.c == ':')
		{
			return actionLook(creature->getPos());
		}
		// pick up an item
		else if (state == STATE_DEFAULT && key.c == ',')
		{
			return actionPickup();
		}
		// pick from multiple items
		else if (state == STATE_PICKUP)
		{
			if (world.itemSelection.keyInput(key))
			{
				int time = 0;
				std::vector<Item*> items = world.itemSelection.getSelection();
				for (std::vector<Item*>::iterator it = items.begin(); it != items.end(); it++)
				{
					time = std::max(time, actionPickup(*it));
				}
				state = STATE_DEFAULT;
				return time;
			}
			return 0;
		}
		// open inventory screen
		else if (state == STATE_DEFAULT && key.c == 'i')
		{
			state = STATE_INVENTORY;
			world.itemSelection = ItemSelection(inventory, "Inventory", false);
			world.itemSelection.compile(world.viewLevel.height - world.viewLevel.height / 4 - 6);
			return 0;
		}
		// open wield weapon screen
		else if (state == STATE_DEFAULT && key.c == 'w')
		{
			world.itemSelection = ItemSelection(inventory, "What do you want to wield?", false);
			world.itemSelection.filterType(ITEM_WEAPON)->runFilter();
			if (world.itemSelection.getNumChoices() > 0)
			{
				world.itemSelection.compile(world.viewLevel.height - world.viewLevel.height / 4 - 6);
				state = STATE_WIELD;
			}
			else
			{
				world.addMessage("You aren't carrying any weapons.");
			}
			return 0;
		}
		// handle inventory
		else if (state == STATE_INVENTORY)
		{
			if (world.itemSelection.keyInput(key))
			{
				// Do something with the item
				// Item* item = world.itemSelection.getItem();
				state = STATE_DEFAULT;
			}
			return 0;
		}
		// handle wield window
		else if (state == STATE_WIELD)
		{
			if (world.itemSelection.keyInput(key))
			{
				state = STATE_DEFAULT;
				return actionWield(world.itemSelection.getItem());
			}
			return 0;
		}
	}
	while (!TCODConsole::isWindowClosed() && !world.requestQuit);
	// should not be reached, return time of action
	return 1;
}

STATE Player::getState()
{
	return state;
}

int Player::computeAttackBonus(Weapon* w)
{
	return (skills[w->getSkill()].value + skills[SKILL_MELEE_COMBAT].value) / 2;
}