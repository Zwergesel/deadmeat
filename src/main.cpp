#include <libtcod.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <deque>
#include <iostream>
#include <cassert>
#include "levelgen.hpp"
#include "world.hpp"
#include "utility.hpp"
#include "player.hpp"
#include "creature.hpp"
#include "level.hpp"
#include "items/weapon.hpp"
#include "monsterfactory.hpp"

World world;
MonsterFactory monsterfactory;

Point getRandomLocation(Level* lev)
{
	TCODRandom* rng = TCODRandom::getInstance();
	Point p;
	do
	{
		p.x = rng->getInt(0, lev->getWidth()-1);
		p.y = rng->getInt(0, lev->getHeight()-1);
	}
	while (!world.tileSet->isPassable(lev->getTile(p)));
	return p;
}

int main()
{
	TCODConsole::initRoot(80,51,"deadmeat",false);
	TCODSystem::setFps(30);

	// Init hardcoded world
	LevelGen level_generator;
	world.viewLevel = Viewport(1, 1, TCODConsole::root->getWidth() - 10, TCODConsole::root->getHeight() - 5);
	world.viewMsg = Viewport(2, TCODConsole::root->getHeight() - 3, TCODConsole::root->getWidth() - 4, 2);
	world.viewItemList = Viewport(
	                       world.viewLevel.x + world.viewLevel.width/8,
	                       world.viewLevel.y + world.viewLevel.height/8,
	                       world.viewLevel.width - world.viewLevel.width/4,
	                       world.viewLevel.height - world.viewLevel.height/4
	                     );

	{
		Savegame save;
		save.loadSavegame("monsters.txt");
	}

	if (false)
	{
		// Add/edit/remove creature templates here
		Goblin goblin(Point(0,0), "goblin", 'g', TCODColor::green, 50);
		monsterfactory.setTemplate("goblin", &goblin);
		Goblin snake(Point(0,0), "snake", 's', TCODColor::darkChartreuse, 25);
		monsterfactory.setTemplate("snake", &snake);
		Goblin dragon(Point(0,0), "red dragon", 'D', TCODColor::red, 350);
		monsterfactory.setTemplate("red dragon", &dragon);
		Savegame save;
		save.beginSave("monsters.txt");
		monsterfactory.save(save);
		save.endSave();
	}

	Savegame save;
	if (save.exists("save.txt"))
	{
		save.loadSavegame("save.txt");
	}
	else if (false && save.exists("defaultworld.txt"))
	{
		save.loadSavegame("defaultworld.txt");
	}
	else
	{
		world.levels[0] = level_generator.generateCaveLevel(80, 50, 40.f);
		world.currentLevel = 0;
		Point newPos = getRandomLocation(world.levels[0]);
		world.player->getCreature()->moveTo(newPos);
		world.levelOffset.x = util::clamp(world.viewLevel.width/2 - newPos.x, world.viewLevel.width - world.levels[0]->getWidth(), 0);
		world.levelOffset.y = util::clamp(world.viewLevel.height/2 - newPos.y, world.viewLevel.height - world.levels[0]->getHeight(), 0);
		Weapon* sword = new Weapon("sword", '(', TCODColor::red, 10, 30, 1, 10, 20, 30, SKILL_SWORD, 1);
		Weapon* dagger = new Weapon("dagger", '(', TCODColor::red, 4, 10, 8, 1, 4, 12, SKILL_DAGGER, 1);
		Item* item1 = new Item("item1", '1', TCODColor::blue);
		Item* item2 = new Item("item2", '2', TCODColor::green);
		Weapon* mace = new Weapon("cursed mace", '(', TCODColor::red, 15, -60, 4, 1, 1, -60, SKILL_MACEFLAIL, 1);
		Armor* uber = new Armor("uber armor", ')', TCODColor::black, 150, 10, SKILL_PLATE_ARMOR);
		Armor* crap = new Armor("crap armor", ')', TCODColor::black, -60, 0, SKILL_LEATHER_ARMOR);
		std::string cr[6] = { "goblin","goblin","goblin","snake","snake","red dragon" };
		for (int i=0; i<6; i++)
		{
			Creature* c = monsterfactory.spawnCreature(cr[i]);
			c->moveTo(getRandomLocation(world.levels[0]));
			world.levels[0]->addCreature(c, 0);
		}
		world.levels[0]->addCreature(world.player->getCreature(), 0);
		world.levels[0]->addItem(dagger, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(item1, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(item2, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(sword, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(mace, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(uber, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(crap, getRandomLocation(world.levels[0]));
	}

	while (!world.requestQuit)
	{
		Level* level = world.levels[world.currentLevel];
		while (!level->isPlayerTurn() && world.getNumMessages() <= 1)
		{
			level->performCreatureTurn();
		}

		// Show new game state
		world.drawWorld();
		// TODO: this somewhat breaks fullscreen, so fix it later
		world.popMessage();
		TCODConsole::root->flush();

		if (world.getNumMessages() > 0)
		{
			// Player has to clear pending messages
			TCOD_key_t key;
			do
			{
				key = world.player->waitForKeypress(true);
			}
			while (key.vk != TCODK_SPACE && !world.requestQuit);
		}
		else
		{
			// This is the player turn made by a player controlled creature
			// We know this because level->isPlayerTurn() must be true here
			level->performCreatureTurn();
		}
	}

	if (world.gameover)
	{
		save.deleteSave("save.txt");
	}
	else
	{
		save.saveWorld(world, "save.txt");
	}

	return 0;
}
