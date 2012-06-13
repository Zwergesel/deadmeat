#include <libtcod.hpp>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include "player.hpp"
#include "creature.hpp"
#include "level.hpp"
#include "world.hpp"
#include "item.hpp"
#include "items/weapon.hpp"
#include "items/food.hpp"
#include "savegame.hpp"

int Player::dx[] = {-1,0,1,-1,0,1,-1,0,1};
int Player::dy[] = {1,1,1,0,0,0,-1,-1,-1};

bool sortCreaturesByDistance(Creature* a, Creature* b);

Player::Player()
{
	// TODO: skills saving & loading
	Skill::setDefaults(skills);
}

Player::Player(std::string name):
	name(name),
	state(STATE_DEFAULT)
{
	Skill::setDefaults(skills);
	TCODRandom* rng = TCODRandom::getInstance();
	attributes[ATTR_STR] = rng->getInt(2,8);
	attributes[ATTR_DEX] = rng->getInt(2,8);
	attributes[ATTR_CON] = rng->getInt(2,8);
	attributes[ATTR_INT] = rng->getInt(2,8);
	level = 1;
	experience = 0;
	attrPoints = 0;
	skillPoints = 3; // TODO: this is for debug
	creature = new Creature(name, F_DEFAULT, (unsigned char)'@', TCODColor::black, 200, 75,
	                        Weapon("bare hands", F_NEUTER | F_PLURAL, '�', TCODColor::pink, 8, 0, 3, 1, 2, 0, 2, EFFECT_NONE, 1), 0, 10, 0
	                       );
	creature->setControlled(true);
	creature->setAttackSkill(/* TODO: Something */0);
	creature->setDefenseSkill(/* TODO: Something */0);
	nutrition = 2500;
}

Player::~Player()
{
}

Creature* Player::getCreature()
{
	return creature;
}

Point Player::getCursor()
{
	return cursor;
}

void Player::addNutrition(int delta)
{
	if (nutrition > HUNGER_NORMAL && nutrition + delta > HUNGER_SATIATED && delta > 0)
	{
		world.addMessage("You feel bloated...");
		world.addMessage("You vomit!", true);
		nutrition = (nutrition + delta) / 2;
		return;
	}
	else if (nutrition < HUNGER_NORMAL && nutrition + delta >= HUNGER_NORMAL)
	{
		world.addMessage("You have a hard time getting it all down.");
	}
	else if (nutrition < HUNGER_WEAK && nutrition + delta >= HUNGER_WEAK && nutrition + delta < HUNGER_HUNGRY)
	{
		world.addMessage("You only feel hungry now.");
	}
	else if (nutrition >= HUNGER_HUNGRY && nutrition + delta < HUNGER_HUNGRY)
	{
		world.addMessage("You are beginning to feel hungry.");
	}
	else if (nutrition >= HUNGER_WEAK && nutrition + delta < HUNGER_WEAK)
	{
		world.addMessage("You are beginning to feel weak.");
	}
	nutrition += delta;
	if (nutrition <= HUNGER_STARVING)
	{
		world.addMessage("You are starving...");
		creature->kill();
	}
}

int Player::getNutrition()
{
	return nutrition;
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
			if (level->getTile(newPos) == TILE_STEPSAME) world.travel();
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
	if (world.fovMap->isInFov(pos.x, pos.y))
	{
		// If square is visible...
		Creature* c = level->creatureAt(pos);
		std::vector<Item*> items = level->itemsAt(pos);
		if (c != NULL && c != creature)
		{
			// ...see the creature there...
			msg << "You see " << util::format(FORMAT_INDEF, c->getName(), c->getFormatFlags()) << " here.";
			world.addMessage(msg.str());
		}
		else if (items.size() == 1)
		{
			// ...or the items if there is no creature
			msg << "You see " << util::format(FORMAT_INDEF, items[0]->toString(), items[0]->getFormatFlags()) << " here.";
			world.addMessage(msg.str());
		}
		else if (items.size() >= 1)
		{
			msg << "You see several items here:";
			world.addMessage(msg.str());
			for (std::vector<Item*>::iterator it=items.begin(); it != items.end();)
			{
				std::stringstream strlist;
				strlist << util::format(FORMAT_INDEF, (*it)->toString(), (*it)->getFormatFlags());
				it++;
				if (it != items.end()) strlist << ",";
				world.addMessage(strlist.str());
			}
		}
		else if (c != NULL && c == creature)
		{
			world.addMessage("You look at yourself...");
			Armor* a = creature->getArmor(ARMOR_BODY);
			if (a == NULL)
			{
				world.addMessage("You think that you look sexy without any body armor.", true);
			}
			else
			{
				msg << "You think that you look sexy in " << util::format(FORMAT_YOUR, a->getName(), a->getFormatFlags()) << ".";
				world.addMessage(msg.str(), true);
			}
		}
		else
		{
			world.addMessage("You don't see anything interesting here.");
		}
	}
	else
	{
		world.addMessage("You cannot see this spot.");
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
	symbol s = creature->addItem(item);
	if (s != 0)
	{
		std::stringstream msg;
		msg << "You pick up [" << s << "] " << util::format(FORMAT_INDEF, item->toString(), item->getFormatFlags()) << ".";
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
		creature->wieldMainWeapon(NULL);
		creature->setAttackSkill(skills[SKILL_ATTACK].value);
	}
	creature->removeItem(item, false);
	msg << "You drop " << util::format(FORMAT_INDEF, item->toString(), item->getFormatFlags()) << ".";
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
		msg << "You are already wielding " << util::format(FORMAT_DEF, weapon->toString(), weapon->getFormatFlags()) << ".";
		world.addMessage(msg.str());
		return 0;
	}
	else
	{
		creature->wieldMainWeapon(weapon);
		creature->setAttackSkill(weapon->getRange() > 1 ? skills[SKILL_ATTACK].value : skills[SKILL_RANGED_ATTACK].value);
		msg << "You are now wielding " << util::format(FORMAT_INDEF, weapon->toString(), weapon->getFormatFlags()) << ".";
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
		msg << "You are already wearing " << util::format(FORMAT_DEF, armor->toString(), armor->getFormatFlags()) << ".";
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
			creature->wearArmor(armor);
			state = STATE_DEFAULT;
			msg << "You finish putting on " << util::format(FORMAT_INDEF, armor->toString(), armor->getFormatFlags()) << ".";
			world.addMessage(msg.str());
			return 0;
		}
	}
	assert(false);
	return 0;
}

int Player::actionTakeoff(Item* item)
{
	if (item == NULL) return 0;

	assert(item->getType() == ITEM_ARMOR);
	Armor* armor = static_cast<Armor*>(item);

	creature->takeOffArmor(armor);

	std::stringstream msg;
	msg << "You take off " << util::format(FORMAT_YOUR, armor->toString(), armor->getFormatFlags()) << ".";
	world.addMessage(msg.str());

	return 50;
}

int Player::actionEat(Item* item)
{
	if (item == NULL) return 0;
	assert(item->getType() == ITEM_FOOD);

	Food* f = static_cast<Food*>(item);

	std::stringstream msg;
	msg << "You eat " << util::format(FORMAT_INDEF, f->toString(), f->getFormatFlags()) << ".";
	world.addMessage(msg.str());

	addNutrition(f->getNutrition());

	creature->removeItem(item, true);

	return 10; // TODO: how long?
}

int Player::actionCharInfo(TCOD_key_t key)
{
	if (key.vk == TCODK_ESCAPE || key.vk == TCODK_SPACE)
	{
		state = STATE_DEFAULT;
		return 0;
	}
	if (attrPoints > 0)
	{
		switch (key.c)
		{
		case 'S':
			attributes[ATTR_STR]++;
			attrPoints--;
			return 0;
		case 'D':
			attributes[ATTR_DEX]++;
			attrPoints--;
			return 0;
		case 'C':
			attributes[ATTR_CON]++;
			attrPoints--;
			return 0;
		case 'I':
			attributes[ATTR_INT]++;
			attrPoints--;
			return 0;
		}
	}
	if (skillPoints > 0)
	{
		int k = util::letterToInt(key.c);
		if (k >= 0 && k < NUM_SKILL)
		{
			// Check whether requirements are met
			if (skills[k].maxValue >= skills[k].maxLevel) return 0;
			std::vector<std::pair<int,int>> r = skills[k].req[skills[k].maxValue + 1];
			for (auto it = r.begin(); it != r.end(); it++)
			{
				if (attributes[it->first] < it->second) return 0;
			}
			// Increase skill
			skills[k].maxValue += 1;
			skillPoints--;
			return 0;
		}
	}
	return 0;
}

void Player::actionAutoTargetting()
{
	bool advanceTarget = false;
	for (auto it = targetList.begin(); it != targetList.end(); it++)
	{
		if ((*it)->getPos() == cursor)
		{
			it++;
			if (it != targetList.end())
			{
				cursor = (*it)->getPos();
				advanceTarget = true;
			}
			break;
		}
	}
	if (!advanceTarget && targetList.size() > 0)
	{
		cursor = targetList.front()->getPos();
	}
}

int Player::actionRangedAttack(Point pos)
{
	Level* level = world.levels[world.currentLevel];
	// Check for valid target
	Creature* target = level->creatureAt(pos);
	if (target == NULL || !world.fovMap->isInFov(pos.x, pos.y))
	{
		world.addMessage("You see nothing to shoot at here.");
		return 0;
	}
	else if (target == creature)
	{
		world.addMessage("After a moment of hesitation you decide not to shoot yourself in the foot.");
		return 0;
	}
	// Check weapon range
	Weapon* w = creature->getMainWeapon();
	if (w->getRange()*w->getRange() < Point::sqlen(creature->getPos() - pos))
	{
		std::stringstream msg;
		msg << "That target is out of range of " << util::format(FORMAT_YOUR, w->getName(), w->getFormatFlags()) << ".";
		world.addMessage(msg.str());
		return 0;
	}
	state = STATE_DEFAULT;
	// Disruption by creatures standing next to the player
	TCODRandom* rng = TCODRandom::getInstance();
	rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);
	for (int i=0; i<9; i++)
	{
		if (i == 4) i++;
		Creature* c = level->creatureAt(creature->getPos() + Point(dx[i], dy[i]));
		if (c != NULL /* TODO: && c->isHostile() && !c->isParalyzed() */)
		{
			if (rng->getInt(0,99) < 50)
			{
				std::stringstream msg;
				msg << util::format(FORMAT_DEF, c->getName(), c->getFormatFlags(), true) << " interrupts your shot.";
				world.addMessage(msg.str());
				// Half shot time when interrupted
				return static_cast<int>(w->getSpeed() + Creature::FACT_ATSPD * creature->getHindrance()) / 2;
			}
		}
	}
	// Shoot
	return creature->rangedAttack(target, w);
}

int Player::action()
{
	// health regeneration
	creature->regenerate(skills[SKILL_HEALTH].value);

	int time = Player::processAction();

	// hunger
	addNutrition(-time);

	return time;
}

int Player::processAction()
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
			if (world.drawBlockingWindow("Save & Quit", "Quit game without saving?\n\n[y]es / [n]o", "yn", TCODColor::red) == 'y')
			{
				world.gameover = true;
				world.requestQuit = true;
			}
			return 0;
		}
		// save&quit
		if (state == STATE_DEFAULT && key.c == 'S')
		{
			if (world.drawBlockingWindow("Save & Quit", "Save game and quit?\n\n[y]es / [n]o", "yn") == 'y')
			{
				world.requestQuit = true;
			}
			return 0;
		}
		// message log
		else if (state == STATE_DEFAULT && key.c == 'L')
		{
			world.drawMessageLog();
			return 0;
		}
		// help screen
		else if (state == STATE_DEFAULT && key.c == 'H')
		{
			world.drawBlockingWindow("Help", HELP_TEXT, " ", TCODColor::blue, false);
			return 0;
		}
		// numpad player movement
		else if (state == STATE_DEFAULT && key.vk >= TCODK_KP1 && key.vk <= TCODK_KP9 && key.vk != TCODK_KP5)
		{
			return actionMove(key.vk - TCODK_KP1);
		}
		// number keys player movement
		else if (state == STATE_DEFAULT && key.vk >= TCODK_1 && key.vk <= TCODK_9 && key.vk != TCODK_5)
		{
			return actionMove(key.vk - TCODK_1);
		}
		// up/down player movement
		else if (state == STATE_DEFAULT && key.c == '<')
		{
			if (world.levels[world.currentLevel]->getTile(creature->getPos()) == TILE_STEPUP)
			{
				world.travel();
				return 10;
			}
			return 0;
		}
		else if (state == STATE_DEFAULT && key.c == '>')
		{
			if (world.levels[world.currentLevel]->getTile(creature->getPos()) == TILE_STEPDOWN)
			{
				world.travel();
				return 10;
			}
			return 0;
		}
		// wait/search
		else if (state == STATE_DEFAULT && (key.vk == TCODK_5 || key.vk == TCODK_KP5))
		{
			return 10;
		}
		// number pad cursor movement
		else if ((state == STATE_INSPECT || state == STATE_RANGED_ATTACK) && key.vk >= TCODK_KP1 && key.vk <= TCODK_KP9 && key.vk != TCODK_KP5)
		{
			moveCursor(key.vk - TCODK_KP1);
			return 0;
		}
		// number keys cursor movement
		else if ((state == STATE_INSPECT || state == STATE_RANGED_ATTACK) && key.vk >= TCODK_1 && key.vk <= TCODK_9 && key.vk != TCODK_5)
		{
			moveCursor(key.vk - TCODK_1);
			return 0;
		}
		else if ((state == STATE_INSPECT || state == STATE_RANGED_ATTACK) && key.vk == TCODK_ESCAPE)
		{
			state = STATE_DEFAULT;
			return 0;
		}
		// Auto-targetting with ranged attack cursor
		else if (state == STATE_RANGED_ATTACK && key.c == 'x')
		{
			actionAutoTargetting();
			return 0;
		}
		// fire a ranged weapon
		else if (state == STATE_RANGED_ATTACK && (key.c == '.' || key.vk == TCODK_ENTER))
		{
			return actionRangedAttack(cursor);
		}
		// initiate ranged attacking
		else if (state == STATE_DEFAULT && key.c == 'f')
		{
			Weapon* w = creature->getMainWeapon();
			if (w == NULL || w->getRange() <= 1)
			{
				world.addMessage("You are not wielding a ranged weapon.");
				return 0;
			}
			state = STATE_RANGED_ATTACK;
			targetList = world.levels[world.currentLevel]->getVisibleCreatures();
			if (targetList.size() > 0)
			{
				sort(targetList.begin(), targetList.end(), sortCreaturesByDistance);
				cursor = targetList.front()->getPos();
			}
			else
			{
				cursor = creature->getPos();
			}
			return 0;
		}
		// look at a different position
		else if (state == STATE_DEFAULT && key.c == ';')
		{
			state = STATE_INSPECT;
			cursor = creature->getPos();
			return 0;
		}
		else if (state == STATE_INSPECT && (key.c == '.' || key.vk == TCODK_ENTER))
		{
			state = STATE_DEFAULT;
			return actionLook(cursor);
		}
		else if (state == STATE_INSPECT && key.vk == TCODK_ESCAPE)
		{
			state = STATE_DEFAULT;
			return 0;
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
		// charater screen
		else if (state == STATE_DEFAULT && key.c == 'C')
		{
			state = STATE_CHARINFO;
			return 0;
		}
		else if (state == STATE_CHARINFO)
		{
			return actionCharInfo(key);
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
		// open eat screen
		else if (state == STATE_DEFAULT && key.c == 'e')
		{
			world.itemSelection = ItemSelection(creature->getInventory(), "What do you want to eat?", false);
			world.itemSelection.filterType(ITEM_FOOD)->runFilter();
			if (world.itemSelection.getNumChoices() > 0)
			{
				world.itemSelection.compile(world.viewItemList.height);
				state = STATE_EAT;
			}
			else
			{
				world.addMessage("You aren't carrying any food.");
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
		// handle eat window
		else if (state == STATE_EAT)
		{
			if (world.itemSelection.keyInput(key))
			{
				state = STATE_DEFAULT;
				return actionEat(world.itemSelection.getItem());
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
				request.append(item->toString());
				request.append(":");
				request.append("\n\nd - drop");
				if (item->getType() == ITEM_FOOD)
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
				char reply = world.drawBlockingWindow("What do you want to do?", request, options);
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
				else if (reply == 'e')
				{
					state = STATE_DEFAULT;
					return actionEat(item);
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

Skill Player::getSkill(SKILLS skill)
{
	return skills[skill];
}

void Player::moveCursor(int dir)
{
	Point cnew = Point(cursor.x + dx[dir], cursor.y + dy[dir]);
	Level* lev = world.levels[world.currentLevel];
	if (cnew.x >= 0 && cnew.y >= 0 && cnew.x < lev->getWidth() && cnew.y < lev->getHeight()
	    && cnew.x >= -world.levelOffset.x && cnew.y >= -world.levelOffset.y
	    && cnew.x < world.viewLevel.width - world.levelOffset.x
	    && cnew.y < world.viewLevel.height - world.levelOffset.y)
	{
		cursor = cnew;
	}
}

void Player::incExperience(int exp)
{
	experience += exp;
	while (experience >= getNeededExp())
	{
		levelUp();
	}
	int numSkillsInTraining = 0;
	for (int i=0; i<NUM_SKILL; i++) if (skills[i].value < skills[i].maxValue) numSkillsInTraining++;
	while (exp >= numSkillsInTraining && numSkillsInTraining > 0)
	{
		// some xp points could get lost here
		int inc = exp / numSkillsInTraining;
		for (int i=0; i<NUM_SKILL; i++)
		{
			if (skills[i].value < skills[i].maxValue)
			{
				int actualInc = std::min(inc, Skill::expNeeded(skills[i].maxValue - 1) - skills[i].exp);
				skills[i].exp += actualInc;
				while (skills[i].exp  >= Skill::expNeeded(skills[i].value))
				{
					skills[i].value++;
					std::stringstream msg;
					msg << "You feel more skilled in " << skills[i].name << ".";
					world.addMessage(msg.str());
					// Update attack and defense skill in creature
					if (i == SKILL_ATTACK) creature->setAttackSkill(skills[i].value);
					if (i == SKILL_DEFENSE) creature->setDefenseSkill(skills[i].value);
					if (i == SKILL_HEALTH) creature->addMaxHealth(10);
				}
				if (skills[i].value == skills[i].maxValue) numSkillsInTraining--;
				assert(skills[i].value <= skills[i].maxValue);
				exp -= actualInc;
			}
		}
	}
}

int Player::getLevel()
{
	return level;
}

int Player::getExperience()
{
	return experience;
}

int Player::getNeededExp()
{
	return 500 * (level+1) * level;
}

void Player::levelUp()
{
	level++;
	attrPoints++;
	skillPoints+=3;
	std::stringstream msg;
	msg << "You are now level " << level << "!";
	world.addMessage(msg.str(), true);
}

int Player::getAttributePoints()
{
	return attrPoints;
}

int Player::getSkillPoints()
{
	return skillPoints;
}

std::string Player::getName()
{
	return name;
}

int Player::rollBonusDamage()
{
	TCODRandom* rng = TCODRandom::getInstance();
	rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);
	return 2*skills[SKILL_DAMAGE].value + rng->getInt(0,skills[SKILL_DAMAGE].value);
}

float Player::getWeaponSpeedBonus()
{
	return 1.0f - 0.05f * skills[SKILL_WEAPON_SPEED].value;
}

bool sortCreaturesByDistance(Creature* a, Creature* b)
{
	Point ppos = world.player->getCreature()->getPos();
	int da = Point::sqlen(a->getPos() - ppos);
	int db = Point::sqlen(b->getPos() - ppos);
	return da < db;
}

const std::string Player::HELP_TEXT =
  "1-9 - Walk / Move cursor\n"
  "5 - Wait for a second\n"
  ": - Look at items at your location\n"
  "; - Look at items/monsters somewhere else\n"
  ", - Pick up item\n"
  ". - Confirm Location\n"
  "d - Drop items\n"
  "e - Eat\n"
  "f - Fire a ranged weapon\n"
  "i - Open inventory\n"
  "w - Wield a weapon\n"
  "W - Wear armor\n"
  "T - Take off armor\n"
  "x - Cycle targets (when using 'f')\n\n"
  "C - Character info\n"
  "L - Message Log\n"
  "S - Save and quit the game\n"
  "Q - Quit and abandon the game\n"
  "Space - Continue\nEsc - Cancel";

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Player::save(Savegame& sg)
{
	// saving resets the state
	state = STATE_DEFAULT;
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Player", id);
	store ("name", name) .ptr("creature", creature->save(sg));
	store ("state", (int) state) ("nutrition", nutrition);
	store ("strength", attributes[ATTR_STR]) ("dexterity", attributes[ATTR_DEX]);
	store ("intelligence", attributes[ATTR_INT]) ("constitution", attributes[ATTR_CON]);
	store ("level", level) ("experience", experience);
	store ("attrPoints", attrPoints) ("skillPoints", skillPoints);
	for (int i=0; i<NUM_SKILL; i++)
	{
		std::stringstream ss;
		ss << "skill" << i;
		store (ss.str(), skills[i].value, skills[i].maxValue, skills[i].exp);
	}
	sg << store;
	return id;
}

void Player::load(LoadBlock& load)
{
	load ("name", name);
	creature = static_cast<Creature*>(load.ptr("creature"));
	int s;
	load ("state", s) ("nutrition", nutrition);
	if (s < 0 || s >= NUM_STATE) throw SavegameFormatException("Player::load _ illegal state");
	state = static_cast<STATE>(s);
	load ("strength", attributes[ATTR_STR]) ("dexterity", attributes[ATTR_DEX]);
	load ("intelligence", attributes[ATTR_INT]) ("constitution", attributes[ATTR_CON]);
	load ("level", level) ("experience", experience);
	load ("attrPoints", attrPoints) ("skillPoints", skillPoints);
	for (int i=0; i<NUM_SKILL; i++)
	{
		std::stringstream ss;
		ss << "skill" << i;
		load (ss.str(), skills[i].value, skills[i].maxValue, skills[i].exp);
	}
}