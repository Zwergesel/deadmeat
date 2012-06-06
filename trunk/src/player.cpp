#include <libtcod.hpp>
#include <sstream>
#include <iostream>
#include <cassert>
#include <cmath>
#include "player.hpp"
#include "creature.hpp"
#include "level.hpp"
#include "world.hpp"
#include "item.hpp"
#include "items/weapon.hpp"
#include "savegame.hpp"

int Player::dx[] = {-1,0,1,-1,0,1,-1,0,1};
int Player::dy[] = {1,1,1,0,0,0,-1,-1,-1};

Player::Player()
{
	// TODO: remove and load in load(..);
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
}

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
	TCODRandom* rng = TCODRandom::getInstance();
	attributes[ATTR_STR] = rng->getInt(5,20);
	attributes[ATTR_DEX] = rng->getInt(5,20);
	attributes[ATTR_CON] = rng->getInt(5,20);
	attributes[ATTR_INT] = rng->getInt(5,20);
	creature = new Creature(name, (unsigned char)'@', TCODColor::black, 250, 75,
	                        Weapon("bare hands", '¤', TCODColor::pink, 8, 0, 3, 1, 2, 0, SKILL_UNARMED, 2), 0, 10
	                       );
	creature->setControlled(true);
	creature->setAttackSkill(skills[SKILL_UNARMED].value);
	creature->setDefenseSkill(skills[SKILL_UNARMORED].value);
}

Player::~Player()
{
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
			if (world.getNumMessages() > 0) world.clearMessage = true;
			return key;
		}
	}
	return TCOD_key_t();
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
			float diagonal = ((newPos - ppos).x != 0 && (newPos - ppos).y != 0)?(std::sqrt(2.f)):(1.f);
			creature->moveTo(newPos);
			world.levelOffset.x = util::clamp(world.viewLevel.width/2 - newPos.x, world.viewLevel.width - level->getWidth(), 0);
			world.levelOffset.y = util::clamp(world.viewLevel.height/2 - newPos.y, world.viewLevel.height - level->getHeight(), 0);
			return static_cast<int>(static_cast<float>(creature->getWalkingSpeed()) * diagonal);
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
		msg << "You see several items here:";
		world.addMessage(msg.str());
		for (std::vector<Item*>::iterator it=items.begin(); it != items.end();)
		{
			std::stringstream strlist;
			strlist << util::indefArticle((*it)->getName()) << " " << (*it)->getName();
			it++;
			if (it != items.end()) strlist << ",";
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
		world.addMessage("There's nothing here.");
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
		world.itemSelection.compile(world.viewItemList.height);
		return 0;
	}
	return 0;
}

int Player::actionPickup(Item* item)
{
	Level* level = world.levels[world.currentLevel];
	if (creature->addItem(item))
	{
		std::stringstream msg;
		msg << "You pick up " << util::indefArticle(item->getName()) << " " << item->getName() << ".";
		world.addMessage(msg.str());
		level->removeItem(item, false);
		return 10;
	}
	return 0;
}

int Player::actionDrop()
{
	world.itemSelection = ItemSelection(creature->getInventory(), "Drop what?", true);
	world.itemSelection.compile(world.viewItemList.height);
	if (world.itemSelection.getNumChoices() <= 0)
	{
		world.addMessage("You're not carrying any items.");
		return 0;
	}
	state = STATE_DROP;
	return 0;
}

int Player::actionDrop(Item* item)
{
	Level* level = world.levels[world.currentLevel];
	std::stringstream msg;
	if (item->getType() == ITEM_ARMOR && creature->getArmor(static_cast<Armor*>(item)->getSlot()) == item)
	{
		msg << "You have to take off your armor first.";
		world.addMessage(msg.str());
		return 0;
	}
	if (creature->getMainWeapon() == item)
	{
		creature->wieldMainWeapon(NULL, skills[SKILL_UNARMED].value);
	}
	creature->removeItem(item, false);
	msg << "You drop " << util::indefArticle(item->getName()) << " " << item->getName() << ".";
	world.addMessage(msg.str());
	level->addItem(item, creature->getPos());
	return 10;
}

int Player::actionWield(Item* itemObj)
{
	std::stringstream msg;
	if (itemObj == NULL) return 0;
	assert(itemObj->getType() == ITEM_WEAPON);

	Weapon* weapon = static_cast<Weapon*>(itemObj);
	if (creature->getMainWeapon() == weapon)
	{
		msg << "You are already wielding " << util::indefArticle(weapon->toString()) << " " << weapon->toString() << ".";
		world.addMessage(msg.str());
		return 0;
	}
	else
	{
		creature->wieldMainWeapon(weapon, computeAttackBonus(weapon));
		msg << "You are now wiedling " << util::indefArticle(weapon->toString()) << " " << weapon->toString() << ".";
		world.addMessage(msg.str());
		return 30;
	}
}

int Player::actionWear(Item* itemObj)
{
	std::stringstream msg;
	if (itemObj == NULL)
	{
		state = STATE_DEFAULT;
		return 0;
	}
	assert(itemObj->getType() == ITEM_ARMOR);

	Armor* armor = static_cast<Armor*>(itemObj);
	if (creature->getArmor(armor->getSlot()) == armor)
	{
		state = STATE_DEFAULT;
		msg << "You are already wearing " << util::indefArticle(armor->toString()) << " " << armor->toString() << ".";
		world.addMessage(msg.str(), true);
		return 0;
	}
	else
	{
		if (state == STATE_WEAR)
		{
			state = STATE_DRESSING;
			world.addMessage("You start changing...");
			return 50;
		}
		else if (state == STATE_DRESSING)
		{
			creature->wearArmor(armor, computeArmorBonus(armor));
			state = STATE_DEFAULT;
			msg << "You finish putting on " << util::indefArticle(armor->toString()) << " " << armor->toString() << ".";
			world.addMessage(msg.str());
			return 0;
		}
	}
	assert(false);
	return 0;
}

int Player::actionTakeoff(Item* item)
{
	if (item == NULL)
	{
		// Selection was cancelled
		return 0;
	}

	assert(item->getType() == ITEM_ARMOR);
	Armor* armor = static_cast<Armor*>(item);

	creature->takeOffArmor(armor);
	if (armor->getSlot() == ARMOR_BODY) creature->setDefenseSkill(skills[SKILL_UNARMORED].value);

	std::stringstream msg;
	msg << "You take off your " << armor->toString() << ".";
	world.addMessage(msg.str());

	return 50;
}

int Player::action()
{
	// finish putting armor on
	if (state == STATE_DRESSING)
	{
		return actionWear(world.itemSelection.getItem());
	}

	do
	{
		TCOD_key_t key = waitForKeypress(true);

		// quit/abandon game
		if (state == STATE_DEFAULT && key.c == 'Q')
		{
			if (world.drawBlockingWindow("Save & Quit", "Quit game without saving?\n\n[y]es / [n]o", TCODColor::red, "yn") == 'y')
			{
				world.gameover = true;
				world.requestQuit = true;
			}
			return 0;
		}
		// save&quit
		if (state == STATE_DEFAULT && key.c == 'S')
		{
			if (world.drawBlockingWindow("Save & Quit", "Save game and quit?\n\n[y]es / [n]o", TCODColor::black, "yn") == 'y')
			{
				world.requestQuit = true;
			}
			return 0;
		}
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
		// drop items
		else if (state == STATE_DEFAULT && key.c == 'd')
		{
			return actionDrop();
		}
		else if (state == STATE_DROP)
		{
			if (world.itemSelection.keyInput(key))
			{
				int time = 0;
				std::vector<Item*> items = world.itemSelection.getSelection();
				for (std::vector<Item*>::iterator it = items.begin(); it != items.end(); it++)
				{
					time = std::max(time, actionDrop(*it));
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
			world.itemSelection = ItemSelection(creature->getInventory(), "Inventory", false);
			world.itemSelection.compile(world.viewItemList.height);
			return 0;
		}
		// open wield weapon screen
		else if (state == STATE_DEFAULT && key.c == 'w')
		{
			world.itemSelection = ItemSelection(creature->getInventory(), "What do you want to wield?", false);
			world.itemSelection.filterType(ITEM_WEAPON)->runFilter();
			if (world.itemSelection.getNumChoices() > 0)
			{
				world.itemSelection.compile(world.viewItemList.height);
				state = STATE_WIELD;
			}
			else
			{
				world.addMessage("You aren't carrying any weapons.");
			}
			return 0;
		}
		// open wear armor screen
		else if (state == STATE_DEFAULT && key.c == 'W')
		{
			world.itemSelection = ItemSelection(creature->getInventory(), "What do you want to wear?", false);
			world.itemSelection.filterType(ITEM_ARMOR)->runFilter();
			if (world.itemSelection.getNumChoices() > 0)
			{
				world.itemSelection.compile(world.viewItemList.height);
				state = STATE_WEAR;
			}
			else
			{
				world.addMessage("You aren't carrying any armor.");
			}
			return 0;
		}
		// open take off armor screen
		else if (state == STATE_DEFAULT && key.c == 'T')
		{
			std::map<symbol,Item*> list = creature->getArmor();
			if (list.size() > 0)
			{
				world.itemSelection = ItemSelection(list, "What do you want to take off?", false);
				world.itemSelection.compile(world.viewItemList.height);
				state = STATE_TAKEOFF;
			}
			else
			{
				world.addMessage("You aren't wearing any armor.");
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
		// handle wear window
		else if (state == STATE_WEAR)
		{
			if (world.itemSelection.keyInput(key))
			{
				return actionWear(world.itemSelection.getItem());
			}
			return 0;
		}
		// handle take off window
		else if (state == STATE_TAKEOFF)
		{
			if (world.itemSelection.keyInput(key))
			{
				state = STATE_DEFAULT;
				return actionTakeoff(world.itemSelection.getItem());
			}
			return 0;
		}
		// handle inventory
		else if (state == STATE_INVENTORY)
		{
			if (world.itemSelection.keyInput(key))
			{
				Item* item = world.itemSelection.getItem();
				if (item == NULL)
				{
					state = STATE_DEFAULT;
					return 0;
				}
				std::string options = "dtX";
				std::string request = "Examining a ";
				request.append(item->getName());
				request.append(":");
				request.append("\n\nd - drop");
				if (true /* combestibles */)
				{
					options.append("e");
					request.append("\n\ne - eat");
				}
				request.append("\n\nt - throw");
				if (item->getType() == ITEM_ARMOR && creature->getArmor(static_cast<Armor*>(item)->getSlot()) == item)
				{
					options.append("T");
					request.append("\n\nT - take off");
				}
				if (true /* tools */)
				{
					options.append("u");
					request.append("\n\nu - use");
				}
				if (item->getType() == ITEM_WEAPON)
				{
					options.append("w");
					request.append("\n\nw - wield");
				}
				if (item->getType() == ITEM_ARMOR && creature->getArmor(static_cast<Armor*>(item)->getSlot()) != item)
				{
					options.append("W");
					request.append("\n\nW - wear");
				}
				request.append("\n\nX - destroy");
				char reply = world.drawBlockingWindow("What do you want to do?", request, TCODColor::black, options);
				if (reply == 'd')
				{
					state = STATE_DEFAULT;
					return actionDrop(item);
				}
				else if (reply == 'w')
				{
					state = STATE_DEFAULT;
					return actionWield(item);
				}
				else if (reply == 'W')
				{
					state = STATE_WEAR;
					return actionWear(item);
				}
				else if (reply == 'T')
				{
					state = STATE_DEFAULT;
					return actionTakeoff(item);
				}
			}
			return 0;
		}
	}
	while (!world.requestQuit);

	// this return should only be reached on requestQuit
	return 0;
}

STATE Player::getState()
{
	return state;
}

int Player::getAttribute(ATTRIBUTE attr)
{
	return attributes[attr];
}

int Player::computeAttackBonus(Weapon* w)
{
	return (skills[w->getSkill()].value + skills[SKILL_MELEE_COMBAT].value) / 2;
}

int Player::computeArmorBonus(Armor* a)
{
	return (skills[a->getSkill()].value + skills[SKILL_MELEE_COMBAT].value) / 2;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Player::save(Savegame& sg)
{
	// saving resets the state
	state = STATE_DEFAULT;
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Player", id);
	// TODO : skills
	store ("name", name) .ptr("creature", creature->save(sg));
	store ("state", (int) state);
	store ("strength", attributes[ATTR_STR]) ("dexterity", attributes[ATTR_DEX]);
	store ("intelligence", attributes[ATTR_INT]) ("constitution", attributes[ATTR_CON]);
	sg << store;
	return id;
}

void Player::load(LoadBlock& load)
{
	load ("name", name);
	creature = static_cast<Creature*>(load.ptr("creature"));
	int s;
	load ("state", s);
	if (s < 0 || s >= NUM_STATE) throw SavegameFormatException("Player::load _ illegal state");
	state = static_cast<STATE>(s);
	load ("strength", attributes[ATTR_STR]) ("dexterity", attributes[ATTR_DEX]);
	load ("intelligence", attributes[ATTR_INT]) ("constitution", attributes[ATTR_CON]);
}