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
#include "items/food.hpp"
#include "items/ammo.hpp"
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

int corruptSave(const std::string& fileName)
{
	TCODConsole::root->setDefaultBackground(TCODColor::blue);
	TCODConsole::root->setDefaultForeground(TCODColor::white);
	TCODConsole::root->clear();
	TCODConsole::root->printEx(40, 18, TCOD_BKGND_NONE, TCOD_CENTER, "ERROR - SAVEGAME IS CORRUPT\n\nDELETE CORRUPT SAVEGAME?\n\n[Y]es / [N]o");
	TCODConsole::root->flush();
	while (true)
	{
		TCOD_key_t key = TCODConsole::root->waitForKeypress(true);
		if (key.c == 'Y')
		{
			Savegame::deleteSave(fileName);
			return 0;
		}
		else if (key.c == 'N' || TCODConsole::root->isWindowClosed())
		{
			return 0;
		}
	}
}

int main()
{
	TCODConsole::setCustomFont("Alloy_curses_12x12.png",TCOD_FONT_LAYOUT_ASCII_INROW);
	TCODConsole::initRoot(80,60,"deadmeat",false);
	TCODSystem::setFps(30);

	// Init hardcoded world
	LevelGen level_generator;
	world.viewLevel = Viewport(1, 1, TCODConsole::root->getWidth() - 16, TCODConsole::root->getHeight() - 5);
	world.viewMsg = Viewport(2, TCODConsole::root->getHeight() - 3, TCODConsole::root->getWidth() - 4, 2);
	world.viewInfo = Viewport(TCODConsole::root->getWidth() - 14, 3, 13, TCODConsole::root->getHeight() - 7);
	world.viewItemList = Viewport(
	                       world.viewLevel.x + world.viewLevel.width/8,
	                       world.viewLevel.y + world.viewLevel.height/8,
	                       world.viewLevel.width - world.viewLevel.width/4,
	                       world.viewLevel.height - world.viewLevel.height/4
	                     );

	if (Savegame::exists("monsters.txt"))
	{
		Savegame save;
		if (!save.loadSavegame("monsters.txt")) return corruptSave("monsters.txt");
	}

	if (true)
	{
		// Add/edit/remove creature templates here
		Goblin goblin("goblin", F_MALE, 'g', TCODColor::green, 35, 0,
		              Weapon("claws", F_DEFAULT, '#', TCODColor::pink, 11, 0, 4, 1, 3, 0, 2, EFFECT_NONE, 1), 15, 10, 1000
		             );
		monsterfactory.setTemplate("goblin", &goblin);
		Goblin snake("snake", F_MALE , 's', TCODColor::darkChartreuse, 20, 0,
		             Weapon("teeth", F_DEFAULT, '#', TCODColor::pink, 25, 0, 11, 2, 6, 0, 0, EFFECT_NONE, 1), 0, 3, 1000
		            );
		monsterfactory.setTemplate("snake", &snake);
		Goblin dragon("Smaug", F_PROPER | F_MALE, 'D', TCODColor::red, 300, 100,
		              Weapon("fangs", F_DEFAULT, '#', TCODColor::pink, 15, 50, 19, 3, 7, 0, 2, EFFECT_NONE, 1), 75, 30, 10000
		             );
		monsterfactory.setTemplate("red dragon", &dragon);
		Savegame save;
		save.beginSave("monsters.txt");
		monsterfactory.save(save);
		save.endSave();
	}

	Savegame save;
	if (Savegame::exists("save.txt"))
	{
		if (!save.loadSavegame("save.txt")) return corruptSave("save.txt");
	}
	else
	{
    world.levels[0] = level_generator.generateBSPLevel(40,40, 6, 2.f); // level_generator.generateRoomLevel(80,50);// level_generator.generateCaveLevel(80, 50, 40.f);
		world.currentLevel = 0;
		Point newPos = getRandomLocation(world.levels[0]);
		world.player->getCreature()->moveTo(newPos);
		world.levelOffset.x = util::clamp(world.viewLevel.width/2 - newPos.x, world.viewLevel.width - world.levels[0]->getWidth(), 0);
		world.levelOffset.y = util::clamp(world.viewLevel.height/2 - newPos.y, world.viewLevel.height - world.levels[0]->getHeight(), 0);
		Weapon* sword = new Weapon("sword", F_NEUTER, '(', TCODColor::red, 10, 30, 1, 10, 20, 30, 1, EFFECT_NONE, 1);
		Weapon* dagger = new Weapon("dagger", F_NEUTER, '(', TCODColor::red, 4, 10, 8, 1, 4, 12, 1, EFFECT_NONE, 1);
		Item* item1 = new Item("item1", F_NEUTER | F_AN, '1', TCODColor::blue);
		Item* item2 = new Item("item2", F_NEUTER | F_AN, '2', TCODColor::green);
		Weapon* bow = new Weapon("longbow", F_NEUTER, '(', TCODColor::cyan, 25, 25, 30, 2, 10, 0, 1, EFFECT_NONE, 10);
		Armor* uber = new Armor("uber armor", F_NEUTER | F_AN, ')', TCODColor::black, 150, 2, 10, ARMOR_BODY);
		Armor* crap = new Armor("prussian pickelhaube", F_NEUTER, ')', TCODColor::black, 50, 1, 0, ARMOR_HAT);
		Armor* boots = new Armor("clown shoes", F_NEUTER | F_BOOTS, ')', TCODColor::black, -30, -1, 12, ARMOR_BOOTS);
		Food* food1 = new Food("beefsteak", F_NEUTER, '%', TCODColor::darkOrange, 2500);
		Food* food2 = new Food("meat ball", F_NEUTER, '%', TCODColor::orange, 1000);
		Ammo* arrows = new Ammo("arrow", F_NEUTER, '!', TCODColor::cyan, 0, EFFECT_NONE);
		std::string cr[7] = { "goblin","goblin","goblin","snake","snake","snake","red dragon" };
		for (int i=0; i<7; i++)
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
		world.levels[0]->addItem(bow, world.player->getCreature()->getPos());
		world.levels[0]->addItem(uber, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(crap, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(boots, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(food1, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(food2, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(food2->clone(), getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(food2->clone(), getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(food2->clone(), getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(food2->clone(), getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(arrows, world.player->getCreature()->getPos());
	}
	world.fovMap = new TCODMap(world.levels[0]->getWidth(), world.levels[0]->getHeight());
	for (int x=0; x<world.levels[0]->getWidth(); x++)
		for (int y=0; y<world.levels[0]->getHeight(); y++)
			world.fovMap->setProperties(x,y,world.tileSet->isPassable(world.levels[0]->getTile(Point(x,y))),world.tileSet->isPassable(world.levels[0]->getTile(Point(x,y))));

	while (!world.requestQuit)
	{
		Level* level = world.levels[world.currentLevel];
		while (!level->isPlayerTurn() && world.getNumMessages() <= 1 && !world.gameover)
		{
			level->performCreatureTurn();
		}

		// Show new game state
		world.drawWorld();
		TCODConsole::root->flush();

		if (world.getNumMessages() > 1)
		{
			// Player has to clear pending messages
			TCOD_key_t key;
			do
			{
				key = world.player->waitForKeypress(true);
			}
			while (key.vk != TCODK_SPACE && !world.requestQuit);
			world.popMessage();
		}
		else if (world.gameover)
		{
			// Game over
			world.drawBlockingWindow("GAME OVER", "You are dead!", " ", TCODColor::red);
			world.requestQuit = true;
		}
		else
		{
			// This is the player turn made by a player controlled creature
			// We know this because level->isPlayerTurn() must be true here
			int nMsg = world.getNumMessages();
			level->performCreatureTurn();
			if (nMsg > 0) world.popMessage();
		}
	}

	if (world.gameover)
	{
		Savegame::deleteSave("save.txt");
	}
	else
	{
		save.saveWorld(world, "save.txt");
	}

	return 0;
}