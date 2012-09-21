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
#include "items/tool.hpp"
#include "items/potion.hpp"
#include "items/spellbook.hpp"
#include "items/corpse.hpp"
#include "savegame.hpp"

int Player::dx[] = {-1, 0, 1,-1, 0, 1,-1, 0, 1};
int Player::dy[] = { 1, 1, 1, 0, 0, 0,-1,-1,-1};


bool sortCreaturesByDistance(Creature* a, Creature* b);

Player::Player():
	selectedItem(NULL)
{
	Skill::setDefaults(skills);
}

Player::Player(std::string name):
	name(name),
	state(STATE_DEFAULT),
	selectedItem(NULL)
{
	Skill::setDefaults(skills);
	attributes[ATTR_STR] = rng->getInt(2,8);
	attributes[ATTR_DEX] = rng->getInt(2,8);
	attributes[ATTR_CON] = rng->getInt(2,8);
	attributes[ATTR_INT] = rng->getInt(2,8);
	level = 1;
	experience = 0;
	attrPoints = 0;
	skillPoints = 3;
	creature = NULL;
	nutrition = 4000;
}

Player::~Player()
{
}

Creature* Player::getCreature()
{
	return creature;
}

void Player::setCreature(Creature* c)
{
	creature = static_cast<PlayerCreature*>(c);
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
		world.deathReason = "You starved to death.";
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
#ifdef TCOD_MACOSX
		else if (key.pressed && key.meta && key.c == 'q')
		{
			world.requestQuit = true;
			return TCOD_key_t();
		}
#endif
		else if (key.pressed && (key.lalt || key.ralt) && key.vk == TCODK_ENTER)
		{
			world.toggleFullscreen();
		}
		else if (key.pressed && key.vk == TCODK_F12)
		{
			TCODSystem::saveScreenshot(NULL);
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
			return creature->attack(newPos);
		}
		else if (creature->getStatusStrength(STATUS_BEARTRAP) > 0)
		{
			if (rng->getInt(0,40) < attributes[ATTR_STR])
			{
				world.addMessage("You escape the bear trap.");
				creature->endStatus(STATUS_BEARTRAP);
			}
			else
			{
				world.addMessage("You're trying to get out of the bear trap.");
			}
			return 15;
		}
		else if (creature->getStatusStrength(STATUS_IMMOBILE) > 0)
		{
			world.addMessage("You try to move, but can't.");
		}
		else if (level->isWalkable(newPos))
		{
			Object* obj = level->objectAt(newPos);
			// if object is invisible, roll for caution
			if (obj != NULL && !obj->isVisible())
			{
				// skill/33 chance of getting it
				if (3 * skills[SKILL_TRAPS].value >= rng->getInt(0, 100))
				{
					std::stringstream ss;
					ss << "Caution! There is " << util::format(FORMAT_INDEF, obj) << " here!";
					world.addMessage(ss.str(), true);
					return 0;
				}
			}

			float diagonal = ((newPos - ppos).x != 0 && (newPos - ppos).y != 0)?(std::sqrt(2.f)):(1.f);
			creature->moveTo(newPos);
			level = world.levels[world.currentLevel];
			world.levelOffset.x = util::clamp(world.viewLevel.width/2 - creature->getPos().x, world.viewLevel.width - level->getWidth(), 0);
			world.levelOffset.y = util::clamp(world.viewLevel.height/2 - creature->getPos().y, world.viewLevel.height - level->getHeight(), 0);
			quickLook();
			return static_cast<int>(static_cast<float>(creature->getWalkingSpeed()) * diagonal);
		}
	}
	return 0;
}

int Player::actionLook(Point pos)
{
	Level* level = world.levels[world.currentLevel];
	if (world.fovMap->isInFov(pos.x, pos.y))
	{
		// If square is visible...
		Creature* c = level->creatureAt(pos);
		std::vector<Item*> items = level->itemsAt(pos);
		Object* obj = level->objectAt(pos);

		if (c != NULL && c != creature)
		{
			// ...see the creature there...
			std::stringstream msg;
			msg << "You see " << util::format(FORMAT_INDEF, c) << " here.";
			world.addMessage(msg.str());
		}
		else if (items.size() == 1)
		{
			// ...or the items if there is no creature
			std::stringstream msg;
			msg << "You see " << util::format(FORMAT_INDEF, items.front()) << " here.";
			world.addMessage(msg.str());
		}
		else if (items.size() > 1)
		{
			std::stringstream msg;
			msg << "You see the following items here:";
			world.addMessage(msg.str());
			for (std::vector<Item*>::iterator it=items.begin(); it != items.end();)
			{
				std::stringstream strlist;
				strlist << util::format(FORMAT_INDEF, *it);
				it++;
				strlist << (it == items.end() ? "." : ",");
				world.addMessage(strlist.str());
			}
		}
		else if (obj == NULL && c != NULL && c == creature)
		{
			// The player is here but nothing else
			world.addMessage("There is nothing here.");
		}
		else if (obj == NULL || !obj->isVisible())
		{
			// Is there nothing at all here, then describe the tile
			std::stringstream msg;
			msg << "You see " << world.tileSet->getDescription(level->getTile(pos)) << " here.";
			world.addMessage(msg.str());
		}
		if (obj != NULL && obj->isVisible())
		{
			// Always mention the object even if there are creatures or items
			std::stringstream msg;
			msg << "You see " << util::format(FORMAT_INDEF, obj) << " here.";
			world.addMessage(msg.str());
		}
	}
	else
	{
		world.addMessage("You cannot see this spot.");
	}
	return 0;
}

void Player::quickLook()
{
	Level* level = world.levels[world.currentLevel];
	std::vector<Item*> items = level->itemsAt(creature->getPos());
	if (items.size() == 1)
	{
		std::stringstream msg;
		msg << "There " << ((items.front()->getFormatFlags() & F_PLURAL) || (items.front()->getAmount() > 1) ? "are " : "is ");
		msg << util::format(FORMAT_INDEF, items.front()) << " here.";
		world.addMessage(msg.str());
	}
	else if (items.size() > 1)
	{
		std::stringstream msg;
		msg << "There are ";
		if (items.size() == 2) msg << "a couple of";
		else if (items.size() == 3) msg << "a few";
		else if (items.size() <= 5) msg << "a handful of";
		else if (items.size() <= 8) msg << "several";
		else if (items.size() <= 13) msg << "many";
		else msg << "a big pile of";
		msg << " items here.";
		world.addMessage(msg.str());
	}
	// Note: Object will identify itself in Object::onStep()
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
		return actionPickup(items[0], items[0]->getAmount());
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

int Player::actionPickup(Item* item, int num)
{
	Level* level = world.levels[world.currentLevel];
	symbol s = creature->expectedInventoryLetter(item);
	std::stringstream msg;
	if (s != 0)
	{
		level->removeItem(item, num, false);
		msg << "You pick up [" << s << "] " << util::format(FORMAT_INDEF, item) << ".";
		world.addMessage(msg.str());
		item = creature->addItem(item);
		return 10; // TODO: why 10?
	}
	else
	{
		msg << "You don't have enough space in your backpack for " << util::format(FORMAT_INDEF, item) << ".";
		world.addMessage(msg.str());
		return 0;
	}
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

int Player::actionDrop(Item* item, int num)
{
	assert(item != NULL);
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
	else if (creature->getQuiver() == item && item->getAmount() <= num)
	{
		creature->readyQuiver(NULL);
	}
	creature->removeItem(item, num, false);
	// setActive(false) must be after removeItem in case of stack splits
	if (item->getType() == ITEM_WEAPON || item->getType() == ITEM_AMMO) item->setActive(false);
	msg << "You drop " << util::format(FORMAT_INDEF, item) << ".";
	world.addMessage(msg.str());
	level->addItem(item, creature->getPos());
	return 10;
}

int Player::actionWield(Item* itemObj)
{
	std::stringstream msg;
	Weapon* current = creature->getMainWeapon();

	if (itemObj == NULL)
	{
		if (current != NULL)
		{
			current->setActive(false);
			creature->wieldMainWeapon(NULL);
			creature->setAttackSkill(skills[SKILL_ATTACK].value);
			msg << "You put away " << util::format(FORMAT_YOUR, current) << ".";
			world.addMessage(msg.str());
			return 15; // TODO: why 15?
		}
		return 0;
	}

	assert(itemObj->getType() == ITEM_WEAPON);
	Weapon* weapon = static_cast<Weapon*>(itemObj);

	// Wield new weapon
	if (current == weapon)
	{
		msg << "You are already wielding " << util::format(FORMAT_DEF, weapon) << ".";
		world.addMessage(msg.str());
		return 0;
	}
	else
	{
		msg << "You are now wielding " << util::format(FORMAT_INDEF, weapon) << ".";
		world.addMessage(msg.str());
		if (current != NULL) current->setActive(false);
		creature->wieldMainWeapon(weapon);
		creature->setAttackSkill(weapon->getRange() > 1 ? skills[SKILL_RANGED_ATTACK].value : skills[SKILL_ATTACK].value);
		return 30; // TODO: why 30?
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
		msg << "You are already wearing " << util::format(FORMAT_DEF, armor) << ".";
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
			state = STATE_DEFAULT;
			msg << "You finish putting on " << util::format(FORMAT_INDEF, armor) << ".";
			world.addMessage(msg.str());
			creature->wearArmor(armor);
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
	msg << "You take off " << util::format(FORMAT_YOUR, armor) << ".";
	world.addMessage(msg.str());

	return 50;
}

int Player::actionEat(Item* item)
{
	if (item == NULL) return 0;
	assert(item->getType() == ITEM_FOOD || item->getType() == ITEM_CORPSE);

	if (item->getType() == ITEM_FOOD)
	{
		Food* f = static_cast<Food*>(item);

		std::stringstream msg;
		msg << "You eat " << util::format(FORMAT_INDEF, f->toString(), f->getFormatFlags()) << ".";
		world.addMessage(msg.str());

		addNutrition(f->getNutrition());
		int time = f->getEatTime();
		creature->removeItem(item, 1, true);
		return time;
	}
	else
	{
		Corpse* c = static_cast<Corpse*>(item);
		std::stringstream msg;
		msg << util::format(FORMAT_DEF, c, true) << " tastes disgusting, but you eat as much of it as you can.";
		int nu = c->getNutrition();

		if (c->isRotten())
		{
			if (rng->getInt(0,15) <= skills[SKILL_TRAPS].value &&
			    world.drawBlockingWindow("You hesitate...", "You smell a faint odor of decay.\nDo you want to continue eating? y/n", "yn", TCODColor::red) != 'y')
			{
				// No negative effect, if the player smells it in time
				return 0;
			}
			world.addMessage(msg.str());
			world.addMessage("Ugh... That meat was tainted.", true);
			world.addMessage("You feel terribly sick.");
			// TODO: better effect
			creature->addMaxHealth(-25);
			nu /= 2;
		}
		else
		{
			world.addMessage(msg.str());
		}

		addNutrition(nu);
		int time = c->getEatTime();
		creature->removeItem(item, 1, true);
		return time;
	}
}

int Player::actionDrink(Item* item)
{
	if (item == NULL) return 0;
	assert(item->getType() == ITEM_POTION);
	Potion* potion = static_cast<Potion*>(item);

	std::stringstream msg;
	msg << "You drink " << util::format(FORMAT_INDEF, potion->toString(), potion->getFormatFlags()) << ".";
	world.addMessage(msg.str());

	potion->effect(creature);

	creature->removeItem(item, 1, true);
	return 15;
}

int Player::actionUse(Item* item)
{
	if (item == NULL) return 0;
	assert(item->getType() == ITEM_TOOL);

	Tool* tool = static_cast<Tool*>(item);
	if (tool->requiresDirection())
	{
		state = STATE_USE_DIRECTION;
		selectedItem = item;
		world.addMessage("In which direction?");
		return 0;
	}
	else
	{
		Level* level = world.levels[world.currentLevel];
		return tool->use(level, creature->getPos());
	}
}

int Player::actionUse(Item* item, int direction)
{
	if (item == NULL) return 0;
	assert(item->getType() == ITEM_TOOL);

	Tool* tool = static_cast<Tool*>(item);
	Level* level = world.levels[world.currentLevel];
	Point target = creature->getPos() + Point(dx[direction], dy[direction]);

	return tool->use(level, target);
}

int Player::actionCast(SPELL spell)
{
	assert(creature->knowsSpell(spell));
	if (g_spells[spell].isTargeted())
	{
		state = STATE_CAST_TARGET;
		selectedSpell = spell;
		initTargetList();
		return 0;
	}
	else
	{
		return actionCast(spell, Point(0,0));
	}
}

int Player::actionCast(SPELL spell, Point target)
{
	assert(creature->knowsSpell(spell));
	state = STATE_DEFAULT;
	int chance = g_spells[spell].getDifficulty() - 5 * skills[g_spells[spell].getSkill()].value;
	if (creature->getMana().first < g_spells[spell].getManaCost())
	{
		world.addMessage("You don't have enough mana to cast this spell.");
		return 0;
	}
	if (rng->getInt(0, 100) > chance)
	{
		creature->addMana(-g_spells[spell].getManaCost());
		Spell::cast(spell, creature, target);
	}
	else
	{
		creature->addMana(-g_spells[spell].getManaCost()/2);
		std::stringstream msg;
		msg << "You failed to cast " << g_spells[spell].getName() << "!";
		world.addMessage(msg.str());
	}
	return g_spells[spell].getCastTime();

}

int Player::actionRead(Item* item)
{
	if (item == NULL) return 0;
	assert(item->getType() == ITEM_SPELLBOOK);

	SpellBook* book = static_cast<SpellBook*>(item);
	std::stringstream msg;
	creature->learnSpell(book->getSpell());
	msg << "You learned the spell \"" << g_spells[book->getSpell()].getName() << "\"!";
	world.addMessage(msg.str());
	return g_spells[book->getSpell()].getDifficulty();
}

int Player::actionQuiver(Item* item)
{
	if (item == NULL) return 0;
	assert(item->getType() == ITEM_AMMO);

	std::stringstream msg;
	Ammo* current = creature->getQuiver();
	if (item == current)
	{
		current->setActive(false);
		msg << util::format(FORMAT_DEF, current, true);
		msg << ((current->getFormatFlags() & F_PLURAL) || current->getAmount() > 1 ? " are " : " is ");
		msg << "already in your quiver";
		world.addMessage(msg.str());
		current->setActive(true);
		return 0;
	}

	msg << "You ready " << util::format(FORMAT_INDEF, item) << ".";
	world.addMessage(msg.str());

	Ammo* a = static_cast<Ammo*>(item);
	if (current != NULL) current->setActive(false);
	creature->readyQuiver(a);

	return 0;
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

void Player::actionAutoTargeting()
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
	// Check quiver
	if (creature->getQuiver() == NULL)
	{
		world.addMessage("You have no ammunition ready.");
		state = STATE_DEFAULT;
		return 0;
	}
	if (creature->getMainWeapon()->getAmmoType() != AMMO_NONE && creature->getQuiver()->getAmmoType() != creature->getMainWeapon()->getAmmoType())
	{
		world.addMessage("You have no fitting ammunition ready.");
		state = STATE_DEFAULT;
		return 0;
	}
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
		msg << "That target is out of range of " << util::format(FORMAT_YOUR, w) << ".";
		world.addMessage(msg.str());
		return 0;
	}
	state = STATE_DEFAULT;
	// Disruption by creatures standing next to the player
	for (int i=0; i<9; i++)
	{
		if (i == 4) i++;
		Creature* c = level->creatureAt(creature->getPos() + Point(dx[i], dy[i]));
		if (c != NULL /* TODO: && c->isHostile() && !c->isParalyzed() */)
		{
			if (rng->getInt(0,99) < 50)
			{
				std::stringstream msg;
				msg << util::format(FORMAT_DEF, c, true) << " interrupts your shot.";
				world.addMessage(msg.str());
				// Half shot time when interrupted
				return static_cast<int>(w->getSpeed() + Creature::FACT_ATSPD * creature->getHindrance()) / 2;
			}
		}
	}
	// Shoot
	return creature->rangedAttack(pos, w);
}

int Player::actionOpen(int direction)
{
	state = STATE_DEFAULT;
	Level* level = world.levels[world.currentLevel];
	Point target = creature->getPos() + Point(dx[direction], dy[direction]);
	Object* obj = level->objectAt(target);
	if (obj != NULL && obj->getType() == OBJ_DOOR_CLOSED)
	{
		return obj->onUse(level, target);
	}
	else if (obj != NULL && obj->getType() == OBJ_DOOR_LOCKED)
	{
		world.addMessage("The door appears to be locked.");
		return 5;
	}
	else if (obj != NULL && obj->getType() == OBJ_DOOR_OPEN)
	{
		world.addMessage("This door is already open.");
	}
	else
	{
		world.addMessage("You see no door there.");
	}
	return 0;
}

int Player::actionClose(int direction)
{
	state = STATE_DEFAULT;
	Level* level = world.levels[world.currentLevel];
	Point target = creature->getPos() + Point(dx[direction], dy[direction]);
	Object* obj = level->objectAt(target);
	if (obj != NULL && obj->getType() == OBJ_DOOR_OPEN)
	{
		return obj->onUse(level, target);
	}
	else if (obj != NULL && obj->getType() == OBJ_DOOR_CLOSED)
	{
		world.addMessage("This door is already closed.");
	}
	else if (obj != NULL && obj->getType() == OBJ_DOOR_BROKEN)
	{
		world.addMessage("This door is destroyed.");
	}
	else
	{
		world.addMessage("You see no door there.");
	}
	return 0;
}

int Player::getDirection(TCOD_key_t key)
{
	if (key.c >= '1' && key.c <= '9' && key.c != '5') return key.c - '1';
	if (key.vk == TCODK_UP) return 7;
	if (key.vk == TCODK_LEFT) return 3;
	if (key.vk == TCODK_RIGHT) return 5;
	if (key.vk == TCODK_DOWN) return 1;
	return -1;
}

void Player::initTargetList()
{
	Level* level = world.levels[world.currentLevel];
	targetList = level->getVisibleCreatures();
	if (targetList.size() > 0)
	{
		sort(targetList.begin(), targetList.end(), sortCreaturesByDistance);
		cursor = targetList.front()->getPos();
	}
	else
	{
		cursor = creature->getPos();
	}
}

int Player::action()
{
	// health regeneration
	creature->regenerate(skills[SKILL_HEALTH].value, skills[SKILL_MANA_REGEN].value);

	// check if weight slows or immobilizes the player
	if (getWeight() > BURDEN_NORMAL && getWeight() <= BURDEN_BURDENED) creature->affect(STATUS_SLOW, 0, 1, 40);
	if (getWeight() > BURDEN_BURDENED) creature->affect(STATUS_IMMOBILE, 0, 1, 1);

	int time = Player::processAction();

	// hunger, roll for thougness to see if player gets more hungry
	// TODO: is this a good method?
	if ( creature->getRace() != RACE_GOLEM && rng->getInt(0, 40) <= 40 - skills[SKILL_NEG_EFFECT].value ) addNutrition(-time);

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

		int direction = getDirection(key);
		Level* level = world.levels[world.currentLevel];

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
		else if (state == STATE_DEFAULT && key.c == 'S')
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
		else if (state == STATE_DEFAULT && (key.c == 'H' || key.vk == TCODK_F1))
		{
			world.drawBlockingWindow("Help", HELP_TEXT, " ", TCODColor::blue, false);
			return 0;
		}
		// player movement
		else if (state == STATE_DEFAULT && direction >= 0)
		{
			return actionMove(direction);
		}
		// up/down player movement
		else if (state == STATE_DEFAULT && key.c == '<')
		{
			Object* obj = level->objectAt(creature->getPos());
			if (obj != NULL && obj->getType() == OBJ_STAIRSUP)
			{
				world.travel();
				return 0;
			}
			return 0;
		}
		else if (state == STATE_DEFAULT && key.c == '>')
		{
			Object* obj = level->objectAt(creature->getPos());
			if (obj != NULL && obj->getType() == OBJ_STAIRSDOWN)
			{
				world.travel();
				return 0;
			}
			return 0;
		}
		// wait/search
		else if (state == STATE_DEFAULT && key.c == '5')
		{
			return 10;
		}
		// cursor movement
		else if ((state == STATE_INSPECT || state == STATE_RANGED_ATTACK || state == STATE_CAST_TARGET) && direction >= 0)
		{
			moveCursor(direction);
			return 0;
		}
		else if ((state == STATE_INSPECT || state == STATE_RANGED_ATTACK || state == STATE_CAST_TARGET) && key.vk == TCODK_ESCAPE)
		{
			state = STATE_DEFAULT;
			return 0;
		}
		// Auto-targeting with ranged attack cursor
		else if ((state == STATE_RANGED_ATTACK || state == STATE_CAST_TARGET) && key.c == 'x')
		{
			actionAutoTargeting();
			return 0;
		}
		// fire a ranged weapon
		else if (state == STATE_RANGED_ATTACK && (key.c == '.' || key.vk == TCODK_ENTER || key.vk == TCODK_SPACE))
		{
			return actionRangedAttack(cursor);
		}
		// cast a targeted spell
		else if (state == STATE_CAST_TARGET && (key.c == '.' || key.vk == TCODK_ENTER || key.vk == TCODK_SPACE))
		{
			return actionCast(selectedSpell, cursor);
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
			initTargetList();
			return 0;
		}
		// look at a different position
		else if (state == STATE_DEFAULT && key.c == ';')
		{
			state = STATE_INSPECT;
			cursor = creature->getPos();
			return 0;
		}
		else if (state == STATE_INSPECT && (key.c == '.' || key.vk == TCODK_ENTER || key.vk == TCODK_SPACE))
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
				std::vector<std::pair<Item*,int>> items = world.itemSelection.getSelection();
				for (auto it = items.begin(); it != items.end(); it++)
				{
					time = std::max(time, actionPickup(it->first, it->second));
				}
				state = STATE_DEFAULT;
				return time;
			}
			return 0;
		}
		// ready quiver
		else if (state == STATE_DEFAULT && key.c == 'q')
		{
			world.itemSelection = ItemSelection(creature->getInventory(), "Ready your quiver");
			world.itemSelection.filterType(ITEM_AMMO)->runFilter();
			if (world.itemSelection.getNumChoices() <= 0)
			{
				world.addMessage("You aren't carrying any ammunition.");
				return 0;
			}
			world.itemSelection.compile(world.viewItemList.height);
			state = STATE_QUIVER;
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
				std::vector<std::pair<Item*,int>> items = world.itemSelection.getSelection();
				for (auto it = items.begin(); it != items.end(); it++)
				{
					time = std::max(time, actionDrop(it->first, it->second));
				}
				state = STATE_DEFAULT;
				return time;
			}
			return 0;
		}
		// attack square
		else if (state == STATE_DEFAULT && key.c == 'a')
		{
			state = STATE_ATTACK;
			world.addMessage("In which direction?");
			return 0;
		}
		else if (state == STATE_ATTACK && direction >= 0)
		{
			state = STATE_DEFAULT;
			Point target = creature->getPos() + Point(dx[direction], dy[direction]);
			return creature->attack(target);
		}
		// character screen
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
			std::map<symbol,Item*> list = creature->getInventory();
			list['*'] = &PSEUDOITEM_NOTHING;
			world.itemSelection = ItemSelection(list, "What do you want to wield?", false);
			world.itemSelection.filterType(ITEM_WEAPON)->filterType(ITEM_DEFAULT)->runFilter();
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
			world.itemSelection.filterType(ITEM_FOOD)->filterType(ITEM_CORPSE)->runFilter();
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
		// open cast screen
		else if (state == STATE_DEFAULT && key.c == 'z')
		{
			std::vector<SPELL> spells;
			for (int i=0; i<NUM_SPELL; i++) if (creature->knowsSpell(static_cast<SPELL>(i))) spells.push_back(static_cast<SPELL>(i));
			if (spells.size() > 0)
			{
				std::stringstream text;
				std::stringstream keys;
				text << "      Spell                Fail Chance\n\n";
				keys << " ";
				int count = 0;
				for (auto it = spells.begin(); it < spells.end(); it++)
				{
					std::string padding;
					padding.append(20 - g_spells[*it].getName().size(), ' ');
					int chance = util::clamp(g_spells[*it].getDifficulty() - skills[g_spells[*it].getSkill()].value, 0, 100);
					text << util::letters[count] << " - " << g_spells[*it].getName() << padding << chance << "%\n";
					keys << util::letters[count++];
				}
				unsigned char cc = world.drawBlockingWindow("Which spell do you want to cast?", text.str(), keys.str());
				if (cc == ' ' || cc == '\0')
				{
					world.addMessage("Nevermind.");
					return 0;
				}
				return actionCast(spells[util::letterToInt(cc)]);
			}
			else
			{
				world.addMessage("You don't know any spells.");
			}
			return 0;
		}
		// open use tool screen
		else if (state == STATE_DEFAULT && key.c == 'u')
		{
			world.itemSelection = ItemSelection(creature->getInventory(), "What do you want to use?", false);
			world.itemSelection.filterType(ITEM_TOOL)->runFilter();
			if (world.itemSelection.getNumChoices() > 0)
			{
				world.itemSelection.compile(world.viewItemList.height);
				state = STATE_USE;
			}
			else
			{
				world.addMessage("You aren't carrying any tools.");
			}
			return 0;
		}
		// open drink screen
		else if (state == STATE_DEFAULT && key.c == 'D')
		{
			world.itemSelection = ItemSelection(creature->getInventory(), "What do you want to drink?", false);
			world.itemSelection.filterType(ITEM_POTION)->runFilter();
			if (world.itemSelection.getNumChoices() > 0)
			{
				world.itemSelection.compile(world.viewItemList.height);
				state = STATE_DRINK;
			}
			else
			{
				world.addMessage("You aren't carrying any potions.");
			}
			return 0;
		}
		// open read screen
		else if (state == STATE_DEFAULT && key.c == 'r')
		{
			world.itemSelection = ItemSelection(creature->getInventory(), "What do you want to read?", false);
			world.itemSelection.filterType(ITEM_SPELLBOOK)->runFilter();
			if (world.itemSelection.getNumChoices() > 0)
			{
				world.itemSelection.compile(world.viewItemList.height);
				state = STATE_READ;
			}
			else
			{
				world.addMessage("You aren't carrying any books.");
			}
			return 0;
		}
		// open doors / close doors
		else if (state == STATE_DEFAULT && (key.c == 'o' || key.c == 'c'))
		{
			world.addMessage("In which direction?");
			state = key.c == 'o' ? STATE_OPEN : STATE_CLOSE;
			return 0;
		}
		// open doors
		else if (state == STATE_OPEN && direction >= 0)
		{
			return actionOpen(direction);
		}
		// close doors
		else if (state == STATE_CLOSE && direction >= 0)
		{
			return actionClose(direction);
		}
		// use a tool that requires a direction
		else if (state == STATE_USE_DIRECTION && direction >= 0)
		{
			state = STATE_DEFAULT;
			return actionUse(selectedItem, direction);
		}
		// Cancel states that require a direction
		else if ((state == STATE_OPEN || state == STATE_CLOSE || state == STATE_ATTACK || state == STATE_USE_DIRECTION) && key.vk == TCODK_ESCAPE)
		{
			world.addMessage("Never mind!");
			state = STATE_DEFAULT;
			return 0;
		}
		// handle wield window
		else if (state == STATE_WIELD)
		{
			if (world.itemSelection.keyInput(key))
			{
				state = STATE_DEFAULT;
				Item* choice = world.itemSelection.getItem();
				if (choice != NULL) return actionWield(choice == &PSEUDOITEM_NOTHING ? NULL : choice);
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
		// handle quiver window
		else if (state == STATE_QUIVER)
		{
			if (world.itemSelection.keyInput(key))
			{
				state = STATE_DEFAULT;
				return actionQuiver(world.itemSelection.getItem());
			}
			return 0;
		}
		// handle use tool window
		else if (state == STATE_USE)
		{
			if (world.itemSelection.keyInput(key))
			{
				state = STATE_DEFAULT;
				return actionUse(world.itemSelection.getItem());
			}
		}
		// handle drink window
		else if (state == STATE_DRINK)
		{
			if (world.itemSelection.keyInput(key))
			{
				state = STATE_DEFAULT;
				return actionDrink(world.itemSelection.getItem());
			}
		}
		// handle read window
		else if (state == STATE_READ)
		{
			if (world.itemSelection.keyInput(key))
			{
				state = STATE_DEFAULT;
				return actionRead(world.itemSelection.getItem());
			}
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
				if (item->getType() == ITEM_POTION)
				{
					options.append("D");
					request.append("\n\nD - drink");
				}
				if (item->getType() == ITEM_FOOD || item->getType() == ITEM_CORPSE)
				{
					options.append("e");
					request.append("\n\ne - eat");
				}
				if (item->getType() == ITEM_AMMO && creature->getQuiver() != item)
				{
					options.append("q");
					request.append("\n\nq - ready ammunition");
				}
				if (item->getType() == ITEM_SPELLBOOK)
				{
					options.append("r");
					request.append("\n\nr - read");
				}
				request.append("\n\nt - throw");
				if (item->getType() == ITEM_ARMOR && creature->getArmor(static_cast<Armor*>(item)->getSlot()) == item)
				{
					options.append("T");
					request.append("\n\nT - take off");
				}
				if (item->getType() == ITEM_TOOL)
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
					return actionDrop(item, item->getAmount());
				}
				else if (reply == 'w')
				{
					state = STATE_DEFAULT;
					return actionWield(item);
				}
				else if (reply == 'q')
				{
					state = STATE_DEFAULT;
					return actionQuiver(item);
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
				else if (reply == 'u')
				{
					state = STATE_DEFAULT;
					return actionUse(item);
				}
				else if (reply == 'D')
				{
					state = STATE_DEFAULT;
					return actionDrink(item);
				}
				else if (reply == 'r')
				{
					state = STATE_DEFAULT;
					return actionRead(item);
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

void Player::setAttribute(ATTRIBUTE attr, int value)
{
  attributes[attr] = value;
}

Skill Player::getSkill(SKILLS skill)
{
	return skills[skill];
}

SPELL Player::getSelectedSpell()
{
	return selectedSpell;
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

					// Update creature
					Weapon* w = creature->getMainWeapon();
					if (i == SKILL_ATTACK && (w == NULL || w->getRange() <= 1)) creature->setAttackSkill(skills[i].value);
					if (i == SKILL_RANGED_ATTACK && w != NULL && w->getRange() > 1) creature->setAttackSkill(skills[i].value);
					if (i == SKILL_DEFENSE) creature->setDefenseSkill(skills[i].value);
					if (i == SKILL_HEALTH) creature->addMaxHealth(10);
					if (i == SKILL_MANA) creature->addMaxMana(10);
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

void Player::setName(std::string n)
{
	name = n;
}

int Player::rollBonusDamage()
{
	return 2*skills[SKILL_DAMAGE].value + rng->getInt(0,skills[SKILL_DAMAGE].value);
}

float Player::getWeaponSpeedBonus()
{
	return (100 - 5 * skills[SKILL_WEAPON_SPEED].value) / 100.0f;
}

float Player::getMoveSpeedBonus()
{
	return (100 - 5 * skills[SKILL_MOVE_SPEED].value) / 100.0f;
}

float Player::getArmorHindranceReduction()
{
	return (10 - skills[SKILL_ARMOR].value) / 10.0f;
}

int Player::getWeight()
{
	int weight = 0;
	auto inv = creature->getInventory();
	for (auto it = inv.begin(); it!=inv.end(); it++)
	{
		weight += (*it).second->getAmount() * (*it).second->getWeight();
	}
	return weight;
}

void Player::checkCorpses()
{
	auto inv = creature->getInventory();
	std::vector<Corpse*> decay;
	for (auto it = inv.begin(); it != inv.end(); it++)
	{
		if (it->second->getType() == ITEM_CORPSE)
		{
			Corpse* cr = static_cast<Corpse*>(it->second);
			if (cr->isDecaying()) decay.push_back(cr);
		}
	}
	for (auto it = decay.begin(); it != decay.end(); it++)
	{
		creature->removeItem(*it, (*it)->getAmount(), true);
	}
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
  "a - Attack\n"
  "d - Drop items\n"
  "c - Close door\n"
  "e - Eat\n"
  "f - Fire a ranged weapon\n"
  "i - Open inventory\n"
  "o - Open door\n"
  "r - Read a book\n"
  "u - Use a tool\n"
  "w - Wield a weapon\n"
  "W - Wear armor\n"
  "T - Take off armor\n"
  "x - Cycle targets (when using 'f')\n"
  "> - Go up stairs\n"
  "< - Go down stairs\n"
  "\n"
  "C - Character info\n"
  "L - Message Log\n"
  "S - Save and quit the game\n"
  "Q - Quit and abandon the game\n"
  "Space - Continue\nEsc - Cancel";

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Player::save(Savegame& sg)
{
	// saving resets the state, except for dressing
	if (state != STATE_DRESSING) state = STATE_DEFAULT;
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
	creature = static_cast<PlayerCreature*>(load.ptr("creature"));
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